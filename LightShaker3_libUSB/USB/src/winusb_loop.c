/*
 * winusb_loop.c
 *
 *  Created on: 10 Feb 2021
 *      Author: SteDun00
 */


#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "usbd_core.h"
#include "usb.h"
#include "usb_std.h"
#include "ms_os.h"
#include "winusb_loop.h"
#include "stm32f042x6.h"


///@brief	Omicron electronics vendor ID
#define WINUSB_VENDOR_ID				0x156D
///@attention	to be defined
#define WINUSB_PRODUCT_ID				0xAFFE

///@brief	USB manufacturer string
#define WINUSB_MANUFACTURER_STRING		"OMICRON electronics GmbH"
///@brief	USB Product Name
#define WINUSB_PRODUCT_STRING			"LightShaker 3.0"
///@brief	USB serial number
#define WINUSBUSB_SERIAL_STRING			"02-2021"
///@brief	Device Interface GUID generated at https://www.guidgenerator.com/online-guid-generator.aspx
///@note	A '\0' terminator is required, which is provided by using \ "\".
#define WINUSB_DEVICE_INTERFACE_GUID	"{276afa5f-7018-4d49-a486-a2d9004d5919}"


//endpoint setup
#define WINUSB_EP0_MAXPACKET_SIZE	(8)
#define WINUSB_EP1_MAXPACKET_SIZE 	(64)
#define WINUSB_EP2_MAXPACKET_SIZE	(8)
#define WINUSB_EP1_RX 				(0x01)
#define WINUSB_EP1_TX 				(0x82)
#define WINUSB_EP2_TX				(0x83)



//USB application specific requests
#define USB_APPLICATION_REQUEST_INTERRUPT_STATE		0x90
#define USB_APPLICATION_REQUEST_SET_BAUD			0x91
#define USB_APPLICATION_REQUEST_GET_BAUD			0x92
#define USB_APPLICATION_REQUEST_GET_FIFO_STATUS		0x93


//global variables
struct usb_device_state usb_device_state = {0};


//local variables
usbd_device winusb_dev = {0};
uint32_t winusb_controll_buffer[32];
uint8_t fifo[FIFO_SIZE] = {0};
uint32_t fpos = 0;




struct winusb_config{
	struct usb_config_descriptor cfg;
	struct usb_interface_descriptor intf1;
	struct usb_endpoint_descriptor epRX1;
	struct usb_endpoint_descriptor epTX1;
	struct usb_endpoint_descriptor epTX2;
} __attribute__((packed));


/*
 * initialise the USB device descriptor
*/
static const struct usb_device_descriptor device_desc = {
		.bLength = sizeof(struct usb_device_descriptor),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE,
		.bcdUSB = VERSION_BCD(2,0,0),
		.bDeviceClass = USB_CLASS_PER_INTERFACE,
		.bDeviceSubClass = USB_SUBCLASS_NONE,
		.bDeviceProtocol = USB_PROTO_NONE,
		.bMaxPacketSize0 = 0x08,
		.idVendor = 0x156D,
		.idProduct = 0xAFFE,
		.bcdDevice = VERSION_BCD(1, 0, 0),
		.iManufacturer = 0x01,
		.iProduct = 0x02,
		.iSerialNumber = 0x03,
		.bNumConfigurations = 0x01
};


/*
 * initialise the WinUSB configuration descriptor
*/
static const struct winusb_config config_desc = {
		.cfg = {
			.bLength = sizeof(struct usb_config_descriptor),
			.bDescriptorType = USB_DESCRIPTOR_TYPE_CONFIG,
			.wTotalLength = sizeof(struct winusb_config),
			.bNumInterfaces = 1,
			.bConfigurationValue = 1,
			.iConfiguration = NO_DESCRIPTOR,
			.bmAttributes = USB_CFG_ATTR_RESERVED,
			.bMaxPower = USB_CFG_POWER_MA(500)
		},
		.intf1 = {
			.bLength = sizeof(struct usb_interface_descriptor),
			.bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
			.bInterfaceNumber = 0,
			.bAlternateSetting = 0,
			.bNumEndpoints = 3,
			.bInterfaceClass = USB_CLASS_VENDOR,
			.bInterfaceSubClass = USB_SUBCLASS_VENDOR,
			.bInterfaceProtocol = USB_PROTO_VENDOR,
			.iInterface = NO_DESCRIPTOR
		},
		.epRX1 = {
			.bLength = sizeof(struct usb_endpoint_descriptor),
			.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
			.bEndpointAddress = WINUSB_EP1_RX,
			.bmAttributes = USB_EP_TYPE_BULK,
			.wMaxPacketSize = WINUSB_EP1_MAXPACKET_SIZE,
			.bInterval = 0x00
		},
		.epTX1 = {
			.bLength = sizeof(struct usb_endpoint_descriptor),
			.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
			.bEndpointAddress = WINUSB_EP1_TX,
			.bmAttributes = USB_EP_TYPE_BULK,
			.wMaxPacketSize = WINUSB_EP1_MAXPACKET_SIZE,
			.bInterval = 0x00
		},
};


#define MS_VENDOR_CODE		0x20
static const struct usb_string_descriptor ms_os_desc = USB_STRING_DESC("MSFT100\x20");
static const struct usb_string_descriptor language_desc = USB_ARRAY_DESC(USB_LANGUAGE_ID_ENG_US);
static const struct usb_string_descriptor manufacturer_desc_en = USB_STRING_DESC(WINUSB_MANUFACTURER_STRING);
static const struct usb_string_descriptor product_desc_en = USB_STRING_DESC(WINUSB_PRODUCT_STRING);
static const struct usb_string_descriptor serial_desc_en = USB_STRING_DESC(WINUSBUSB_SERIAL_STRING);

static const struct usb_string_descriptor *const dTable[] = {
		&language_desc,
		&manufacturer_desc_en,
		&product_desc_en,
		&serial_desc_en
};


/*
 * initialise the WinUSB device GUID extra property feature descriptor
*/
static const struct winusb_devguid_extprop_feature_descriptor devguid_ext_prop_feature_desc = {
		.dwLength = sizeof(struct winusb_devguid_extprop_feature_descriptor),
		.bcdVersion = VERSION_BCD(1, 0, 0),
		.wIndex = 5,
		.wCount = 1,
		.dev_guid_property = {
			.dwPropertyLength = sizeof(struct winusb_devguid_property),
			.dwPropertyType = 1,
			.wPropertyNameLength = 40,
			.wPropertyName = STRING_TO_WCHAR("DeviceInterfaceGUID"),
			.dwPropertyDataLength = 78,
			.wPropertyData = STRING_TO_WCHAR(WINUSB_DEVICE_INTERFACE_GUID)
		}
};

/*
 * initialise the WinUSB compat ID description
*/
static const struct ms_compat_id_descriptor compatid_desc = {
		.dwLength = sizeof(struct ms_compat_id_descriptor),
		.bcdVersion = VERSION_BCD(1, 0, 0),
		.wIndex = 4,
		.bCount = 1,
		._reservedA = {0, 0, 0, 0, 0, 0, 0},
		.bInterfaceNumber = 0,
		._reservedB = {0},
		.chCompatibleID = {'W', 'I', 'N', 'U', 'S', 'B', 0, 0},
		.chSubCompatibleID = {0, 0, 0, 0, 0, 0, 0, 0},
		._reservedC = {0, 0, 0, 0, 0, 0}
};


static usbd_status_respond winusb_get_desc(usbd_control_request *pReq, void **ppAddress, uint16_t *pLength);
static usbd_status_respond winusb_control(usbd_device *pDev, usbd_control_request *pReq, usbd_req_complete_callback *pCallback);
static usbd_status_respond winusb_set_config(usbd_device *pDev, uint8_t config);
static void bulk_from_host(usbd_device *pDev, uint8_t event, uint8_t ep);
static void bulk_to_host(usbd_device *pDev, uint8_t event, uint8_t ep);


///@brief	function to get the WinUSB device descriptor
///@param	pReq pointer to the USB device control request structure
///@param	ppAddress pointer for the device descriptor
///@param	pLength pointer to return the length of the USB device configuration
///@return	status result of the USB
static usbd_status_respond winusb_get_desc(usbd_control_request *pReq, void** ppAddress, uint16_t *pLength){
	const uint8_t dType = pReq->wValue >> 8;
	const uint16_t dNumber = pReq->wValue & 0xFF;
	const void *pDesc;
	uint16_t len = 0;
	switch(dType){
	case USB_DESCRIPTOR_TYPE_DEVICE:
		pDesc = &device_desc;
		break;
	case USB_DESCRIPTOR_TYPE_CONFIG:
		pDesc = &config_desc;
		len = sizeof(config_desc);
		break;
	case USB_DESCRIPTOR_TYPE_STRING:
		if(dNumber < 4){
			pDesc = dTable[dNumber];
		} else if(dNumber == 0xEE){
			pDesc = &ms_os_desc;
		} else {
			return usbd_failed;
		}
		break;
	default:
		return usbd_failed;
	}
	if(len == 0){
		len = ((struct usb_header_descriptor*)pDesc)->bLength;
	}
	*ppAddress = (void*)pDesc;
	*pLength = len;
	return usbd_acknowledged;
}


///@brief	function set the WinUSB configuration
///@param	pDev pointer to the USB device structure
///@param	config flag to configure (true) or de-configure (false) the USB device
///@return 	status result of the USB
static usbd_status_respond winusb_set_config(usbd_device *pDev, uint8_t config){
	switch(config){
	//de-configure device
	case 0:
		USBD_EP_Deconfig(pDev, WINUSB_EP1_TX);
		USBD_EP_Deconfig(pDev, WINUSB_EP1_RX);
		USBD_EP_Deconfig(pDev, WINUSB_EP2_TX);
		USBD_Register_Endpoint(pDev, WINUSB_EP1_RX, 0);
		USBD_Register_Endpoint(pDev, WINUSB_EP1_TX, 0);
		USBD_Register_Endpoint(pDev, WINUSB_EP2_TX, 0);

		//set to false, because device is not configured
		usb_device_state.isConfigured = false;
		return usbd_acknowledged;


		//configure device
	case 1:
		USBD_EP_Config(pDev, WINUSB_EP1_RX, USB_EP_TYPE_BULK | USB_EP_TYPE_DBL_BUF, WINUSB_EP1_MAXPACKET_SIZE);
		USBD_EP_Config(pDev, WINUSB_EP1_TX, USB_EP_TYPE_BULK | USB_EP_TYPE_DBL_BUF, WINUSB_EP1_MAXPACKET_SIZE);
		USBD_EP_Config(pDev, WINUSB_EP2_TX, USB_EP_TYPE_INTERRUPT, WINUSB_EP2_MAXPACKET_SIZE);
		USBD_Register_Endpoint(pDev, WINUSB_EP1_RX, bulk_from_host);
		USBD_Register_Endpoint(pDev, WINUSB_EP1_TX, bulk_to_host);
		USBD_Register_Endpoint(pDev, WINUSB_EP2_TX, 0);
//
//		USBD_EP_Write(pDev, WINUSB_EP1_TX, 0, 0);
//		USBD_EP_Write(pDev, WINUSB_EP2_TX, 0, 0);
		//set to true, because device is configured
		usb_device_state.isConfigured = true;
		return usbd_acknowledged;

	default:
		return usbd_failed;
	}
//	return usbd_failed;
}


///@brief	function sets the WinUSB control
///@param	pDev pointer to the USB device structure
///@param	pReq pointer to the USB control request structure
///@param 	pCallback pointer to the USB device request completion callback
///@return	status result of the USB

//TODO: It seems that this function is not called - at this point the enumeration process breaks
static usbd_status_respond winusb_control(usbd_device *pDev, usbd_control_request *pReq, usbd_req_complete_callback *pCallback){
	if((pReq->bmRequestType & USB_REQ_TYPE_TYPE) != USB_REQ_TYPE_VENDOR){
		return usbd_failed;
	}
	if(((pReq->bmRequestType & USB_REQ_TYPE_DIRECTION) == USB_REQ_TYPE_DEVTOHOST) || pReq->bmRequestType == (USB_REQ_TYPE_DEVTOHOST | USB_REQ_TYPE_VENDOR | USB_REQ_TYPE_DEVICE)){
		switch(pReq->bRequest){
		case MS_VENDOR_CODE:
			switch(pReq->wIndex){
			case 0x04:
				pDev->deviceStatus.pData = (void*)&compatid_desc;
				pDev->deviceStatus.data_count = sizeof(compatid_desc);
				return usbd_acknowledged;
			case 0x05:
				pDev->deviceStatus.pData = (void*)&devguid_ext_prop_feature_desc;
				pDev->deviceStatus.data_count = sizeof(devguid_ext_prop_feature_desc);
				return usbd_acknowledged;
			default:
				return usbd_failed;
			}
			break;
		case USB_APPLICATION_REQUEST_GET_FIFO_STATUS:
			pDev->deviceStatus.pData = &fpos;
			pDev->deviceStatus.data_count = sizeof(fpos);
			return usbd_acknowledged;
		default:
			return usbd_failed;
		}
	}

	if((pReq->bmRequestType & USB_REQ_TYPE_DIRECTION) == USB_REQ_TYPE_HOSTTODEV){
		switch(pReq->bRequest){
		case USB_APPLICATION_REQUEST_INTERRUPT_STATE:
			usb_device_state.isInterruptEnabled = pReq->wValue > 0 ? true : false;
			return usbd_acknowledged;
		default:
			return usbd_failed;
		}
	}
	return usbd_failed;
}


///@brief	callback to receive data from the host
///@param	pDev pointer to the USB device structure
///@param	event check if the event is set to receive data from the host
///@param	ep endpoint number
///@return	none

//TODO: function not testet - enumeration does not work properly
static void bulk_from_host(usbd_device *pDev, uint8_t event, uint8_t ep){
	if(event == USBD_EVENT_PACKET_RX){
		//TODO: finish function to receive data from host when enumeration works
		if(fpos >= FIFO_SIZE){
			fpos = 0;
		}
		uint32_t dataReceived = USBD_EP_Read(pDev, WINUSB_EP1_RX, &fifo[fpos], WINUSB_EP1_MAXPACKET_SIZE);
		fpos += dataReceived;
	}
}


///@brief	callback for the host to request data from device
///@param	pDev pointer to the USB device structure
///@param	event check if the event is set to receive data from the host
///@param	ep endpoint number
///@return	none
static void bulk_to_host(usbd_device *pDev, uint8_t event, uint8_t ep){
	//TODO: implement function for host request to receive data when enumeration works
}


static struct {
	char msg[6];
	uint8_t interruptFlag;
	uint16_t count;
} __attribute__((packed)) interrupt_data = {0};


///@brief	send data via interrupt to the host
///@param	char string for message (test-message)
///@return	none
void usb_interrupt_send(char *msg){
	strcpy(interrupt_data.msg, msg);
	interrupt_data.count++;
	USBD_EP_Write(&winusb_dev, WINUSB_EP2_TX, &interrupt_data, sizeof(interrupt_data));
}


///@brief	function combines all initialisations for the USB communication
///@param	none
///@return	none
///@note	remap the pins PA11 and PA12 to the GPIOA P8 and P9 for the STM32F042F6P6
void winusb_init_usbd(void){
	//remap the pins for USB communication (DP and DN) - necessarry for the STM32F042F6P6
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
	SYSCFG->CFGR1 |= SYSCFG_CFGR1_PA11_PA12_RMP;

	USBD_Init(&winusb_dev, &usbd_hw, WINUSB_EP0_MAXPACKET_SIZE, winusb_controll_buffer, sizeof(winusb_controll_buffer));
	USBD_Register_Config(&winusb_dev, winusb_set_config);
	USBD_Register_Control(&winusb_dev, winusb_control);
	USBD_Register_Descriptor(&winusb_dev, winusb_get_desc);


	USBD_Enable(&winusb_dev, true);
}


///@brief	function connects the USB device with the host
///@param	none
///@return	none
void usb_device_connect(void){
	USBD_Connect(&winusb_dev, true);
	usb_device_state.isConnected = true;
}


///@brief	function disconnects the USB device from the host
///@param	none
///@return	none
void usb_device_disconnect(void){
	USBD_Connect(&winusb_dev, false);
	usb_device_state.isConnected = false;
}


///@brief	function handles the USB device communication
///@param	none
///@return	none
void USB_Handler(void){
	USBD_Poll(&winusb_dev);
}


