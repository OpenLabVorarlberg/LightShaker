/*
 * @file			usb_std.h
 * @brief			standard definitions for USB implementation with embedded controllers
 * @author			SteDun00
 * @version			V1.0
 * @review date		2021-02-26
 * @note			reference to https://github.com/dmitrystu/libusb_stm32/blob/winusb/inc/usb_std.h
 */

#ifndef USB_INC_USB_STD_H_
#define USB_INC_USB_STD_H_
#if defined(__cplusplus)
	extern "C" {
#endif


#include <inttypes.h>



#define __CAT(x,y) 		x ## y
#define CAT(x,y) 		__CAT(x,y)


///@brief	Macro to encode major/minor/version number into BCD code.
#define VERSION_BCD(maj, min, rev)		(((maj & 0xFF) << 8) | ((min & 0x0F) << 4) | (rev & 0x0F))

///@brief	Macro to create usb_string_descriptor from array
#define USB_ARRAY_DESC(...)				{.bLength = 2 + sizeof((uint16_t[]){__VA_ARGS__}),\
										 .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,\
										 .wString = {__VA_ARGS__}}

///@brief	Macro to create usb_string_descriptor from string
#define USB_STRING_DESC(s)				{.bLength = sizeof(CAT(u,s)),\
										 .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,\
										 .wString = {CAT(u,s)}}

///@brief	Macro to set MAX power consumption field for the usb_config_descriptor
#define USB_CFG_POWER_MA(mA)			((mA) >> 1)


#define USB_CFG_ATTR_RESERVED			0x80
#define USB_CFG_ATTR_SELFPOWERED		0x40


#define USB_EP_DIRECTION_IN				0x00
#define USB_EP_DIRECTION_OUT			0x80
#define USB_EP_TYPE_CONTROL				0x00
#define USB_EP_TYPE_ISYNCHRONOUS		0x01
#define USB_EP_TYPE_BULK				0x02
#define USB_EP_TYPE_INTERRUPT			0x03
#define USB_EP_TYPE_NO_SYNCHRONOUS		0x00
#define USB_EP_ATTRIBUTE_ASYNCHRONOUS	0x04
#define USB_EP_ATTRIBUTE_ADAPTIVE		0x08
#define USB_EP_ATTRIBUTE_SYNCRONOUS		0x0C
#define USB_EP_USAGE_DATA				0x00
#define USB_EP_USAGE_FEEDBACK			0x10
#define USB_EP_USAGE_IMP_FEEDBACK		0x20


///@note	String descriptor does not exist in the device.
#define NO_DESCRIPTOR					0x00

///@note	String descriptor is an internal serial number provided by the hardware driver.
#define INTERNAL_SERIAL_NO_DESCRIPTOR	0xFE


#define USB_CLASS_PER_INTERFACE			0x00
#define USB_CLASS_VENDOR				0xFF
#define USB_SUBCLASS_NONE				0x00
#define USB_SUBCLASS_VENDOR				0xFF
#define USB_PROTO_NONE					0x00
#define USB_PROTO_VENDOR				0xFF
#define USB_CLASS_IAD					0xEF
#define USB_CLASS_APP_SPEC				0xFE
#define USB_SUBCLASS_IAD				0x02
#define USB_PROTO_IAD					0x01


#define USB_DESCRIPTOR_TYPE_DEVICE			0x01
#define USB_DESCRIPTOR_TYPE_CONFIG			0x02
#define USB_DESCRIPTOR_TYPE_STRING			0x03
#define USB_DESCRIPTOR_TYPE_INTERFACE		0x04
#define USB_DESCRIPTOR_TYPE_ENDPOINT		0x05
#define USB_DESCRIPTOR_TYPE_QUALIFIER		0x06
#define USB_DESCRIPTOR_TYPE_OTHER			0x07
#define USB_DESCRIPTOR_TYPE_INTERFACEPOWER	0x08
#define USB_DESCRIPTOR_TYPE_OTG				0x09
#define USB_DESCRIPTOR_TYPE_DEBUG			0x0A
#define USB_DESCRIPTOR_TYPE_INTERFASEASSOC	0x0B
#define USB_DESCRIPTOR_TYPE_CS_INTERFACE	0x24
#define USB_DESCRIPTOR_TYPE_CS_ENDPOINT		0x25


#define USB_STANDARD_GET_STATUS			0x00
#define USB_STANDARD_CLEAR_FEATURE		0x01
#define USB_STANDARD_SET_FEATURE		0x03
#define USB_STANDARD_SET_ADDRESS		0x05
#define USB_STANDARD_GET_DESCRIPTOR		0x06
#define USB_STANDARD_SET_DESCRIPTOR		0x07
#define USB_STANDARD_GET_CONFIG			0x08
#define USB_STANDARD_SET_CONFIG			0x09
#define USB_STANDARD_GET_INTERFACE		0x0A
#define USB_STANDARD_SET_INTERFACE		0x0B
#define USB_STANDARD_SYNC_FRAME			0x0C


#define USB_FEATURE_ENDPOINT_HALT		0x00
#define USB_FEATURE_REMOTE_WAKEUP		0x01
#define USB_FEATURE_TEST_MODE			0x02
#define USB_FEATURE_DEBUG_MODE			0x06


#define USB_TEST_J						0x01
#define USB_TEST_K						0x02
#define USB_TEST_SE0_NAK				0x03
#define USB_TEST_PACKET					0x04
#define USB_TEST_FORCE_ENABLE			0x05


///@brief	contains core API for USBD_CORE
///@note	double-buffered endpoint is only used for bulk endpoint
#define USB_EP_TYPE_DBL_BUF				0x04


#define USB_REQ_TYPE_DIRECTION			0x80
#define USB_REQ_TYPE_DEVTOHOST			0x80
#define USB_REQ_TYPE_HOSTTODEV			0x00
#define USB_REQ_TYPE_TYPE				0x60
#define USB_REQ_TYPE_STANDARD			0x00
#define USB_REQ_TYPE_CLASS				0x20
#define USB_REQ_TYPE_VENDOR				0x40
#define USB_REQ_TYPE_RECIPIENT			0x03
#define USB_REQ_TYPE_DEVICE				0x00
#define USB_REQ_TYPE_INTERFACE			0x01
#define USB_REQ_TYPE_ENDPOINT			0x02
#define USB_REQ_TYPE_OTHER				0x03


///@brief	language ID definition
///@note	Since all the USB functions are in English, it's enough to just define the English language ID
#define USB_LANGUAGE_ID_ENG_US			0x0409



///@brief	USB descriptor header
struct usb_header_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
} __attribute__((packed));


///@brief	USB device descriptor
///@note	general information about an USB device - contains all the device's configurations
struct usb_device_descriptor {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint16_t bcdUSB;
	uint8_t  bDeviceClass;
	uint8_t  bDeviceSubClass;
	uint8_t  bDeviceProtocol;
	uint8_t  bMaxPacketSize0;
	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;
	uint8_t  iManufacturer;
	uint8_t  iProduct;
	uint8_t  iSerialNumber;
	uint8_t  bNumConfigurations;
} __attribute__((packed));


///@brief	USB device qualifier descriptor
///@note	describes information about a high-speed capable device - returns information about how a device would in HS-mode if it's in full-speed mode
struct usb_qualifier_descriptor {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint16_t bcdUSB;
	uint8_t  bDeviceClass;
	uint8_t  bDeviceSubClass;
	uint8_t  bDeviceProtocol;
	uint8_t  bMaxPacketSize0;
	uint8_t  bNumConfigurations;
	uint8_t  bReserved;
} __attribute__((packed));


///@brief	USB device configuration descriptor
///@note	describes information about a specific device configuration
struct usb_config_descriptor {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint16_t wTotalLength;
	uint8_t  bNumInterfaces;
	uint8_t  bConfigurationValue;
	uint8_t	 iConfiguration;
	uint8_t  bmAttributes;
	uint8_t  bMaxPower;
} __attribute__((packed));


///@brief	USB interface descriptor
///@note	describes a specific interface within a configuration
struct usb_interface_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bInterfaceNumber;
	uint8_t bAlternateSetting;
	uint8_t bNumEndpoints;
	uint8_t bInterfaceClass;
	uint8_t bInterfaceSubClass;
	uint8_t bInterfaceProtocol;
	uint8_t iInterface;
};


///@brief	USB interface association descriptor
///@note	IAD allows the device to group interfaces belonging to a function
struct usb_inter_assos_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bFirstInterface;
	uint8_t bInterfaceCount;
	uint8_t bFunctionClass;
	uint8_t bFunctionSubClass;
	uint8_t bFunctionProtocol;
	uint8_t iFunction;
} __attribute__((packed));


///@brief	USB endpoint descriptor
///@note	contains information for the host to determine the bandwidth of each endpoint
struct usb_endpoint_descriptor {
	uint8_t		bLength;
	uint8_t		bDescriptorType;
	uint8_t		bEndpointAddress;
	uint8_t		bmAttributes;
	uint16_t	wMaxPacketSize;
	uint8_t		bInterval;
} __attribute__((packed));


///@brief	USB string descriptor
///@brief	contains one ore more not NULL-terminated Unicode strings; optional - only used if the device does support a string descriptor
struct usb_string_descriptor {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint16_t wString[];
} __attribute__((packed));


///@brief	USB debug descriptor
///@note	provides certain characteristics which the host needs for debug process
struct usb_debug_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDebugInEndpoint;
	uint8_t bDebugOutEndpoint;
} __attribute__((packed));




#if defined(__cplusplus)
	}
#endif


#endif /* USB_INC_USB_STD_H_ */
