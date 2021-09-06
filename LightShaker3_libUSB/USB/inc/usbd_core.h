/*
 * @file			usbd_core.h
 * @brief			definitions and structures for a USB device
 * @author			SteDun00
 * @version			V1.0
 * @review date		2021-02-26
 * @note			with reference to https://github.com/dmitrystu/libusb_stm32/blob/winusb/inc/usbd_core.h
 * OMICRON electronics GmbH, Austria
 */

#ifndef USB_INC_USBD_CORE_H_
#define USB_INC_USBD_CORE_H_

#ifdef __cplusplus
	extern "C"{
#endif

#if defined(__DOXYGEN__)
//name compile-time control macros
#define USBD_SOF_DISABLED
#define USBD_VBUS_DETECT
#define USBD_DP_PORT
#define USBD_DP_PIN
#define USBD_SOF_OUT

#endif

#define USBD_EVENT_RESET				0
#define USBD_EVENT_SOF					1
#define USBD_EVENT_SUSPEND				2
#define USBD_EVENT_WAKEUP				3
#define USBD_EVENT_PACKET_TX			4
#define USBD_EVENT_PACKET_RX			5
#define USBD_EVENT_PACKET_SETUP			6
#define USBD_EVENT_ERROR				7
#define USBD_EVENT_COUNT				8


#define USBD_LANE_UNKNOW				0
#define USBD_LANE_DISCONNECTED			1
#define USBD_LANE_STD_DOWNSTREAM		2
#define USBD_LANE_CARGING_DOWNSTREAM	3
#define USBD_LANE_CARGING_DEDICATED		4


#define USBD_HW_ADDRESS_BEFORE_STATUS_OUT	0x01
#define USBD_HW_BATTERY_CARGING				0x02
#define USBD_HW_HIGH_SPEED_SIPPORTED		0x04
#define USBD_HW_DEVICE_ENABLE				0x08
#define USBD_HW_ENUMERATION_SPEED_MSK		0x30
#define USBD_HW_SPEED_NOT_CONNECTED			0x00
#define USBD_LOW_SPEED						0x10
#define USBD_HW_SPEED_FULL_SPEED			0x20
#define USBD_HW_SPEED_HIGH_SPEED			0x30


#if !defined(__ASSEMBLER__)
#include <stdbool.h>

///@brief	USB device machine states
enum usbd_machine_state {
	usbd_state_disabled,
	usbd_state_disconnected,
	usbd_state_default,
	usbd_state_addressed,
	usbd_state_configured
};


///@brief	USB device control endpoint machine states
enum usbd_control_machine_state {
	usbd_ctl_idle,
	usbd_ctl_data_rx,
	usbd_ctl_data_tx,
	usbd_ctl_data_tx_zero_len_packet,
	usbd_ctl_data_last,
	usbd_ctl_status_in,
	usbd_ctl_status_out
};


///@brief 	asynchrono9us device control commands
enum usbd_commands {
	usbd_cmd_enable,
	usbd_cmd_disable,
	usbd_cmd_connect,
	usbd_cmd_disconnect,
	usbd_cmd_reset
};


///@brief	USB status result
typedef enum _usbd_status_respond_ {
	usbd_failed,
	usbd_acknowledged,
	usbd_not_acknowledged
} usbd_status_respond;


///@brief	USBD device
typedef struct _usbd_device_ usbd_device;


///@brief	USB generic control request
typedef struct _usbd_control_request_ {
	uint8_t  bmRequestType;
	uint8_t  bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
	uint8_t  data[];
} usbd_control_request;


///USB device status data
typedef struct _usbd_device_status_ {
	void	 *pDataBuffer;
	void	 *pData;
	uint16_t data_count;
	uint16_t data_max_size;
	uint8_t  ep0_size;
	uint8_t  device_configuration;
	uint8_t  device_machine_state;
	uint8_t  control_state;
} usbd_device_status;


///@brief	USB device event callback and endpoint processing
///@param	pDev pointer to USB device
///@param	events USB event --> refers to USB_EVENTS
///@param	endpoint USB endpoint number
///@return	none
///@note	endpoints with the same index share the same callback
typedef void (*usbd_event_callback) (usbd_device *pDev, uint8_t events, uint8_t endpoint);


///@brief	USB control transfer completed --> callback function
///@param	pDev pointer to USB device
///@param	pReq pointer to USB request function
///@return 	none
///@note	usbd_device->complete_callback will be set to NULL after this callback is completed
typedef void (*usbd_req_complete_callback) (usbd_device *pDev, usbd_control_request *pReq);


///@brief	USB control callback function
///@param	pDev pointer to USB device
///@param	pReq pointer to USB request function
///@param	pCallback pointer to USB control transfer callback function
///@return	USBD respond status
typedef usbd_status_respond (*usbd_controll_callback) (usbd_device *pDev, usbd_control_request *pReq, usbd_req_complete_callback *pCallback);


///@brief	USB get descriptor callback function (called if get-descriptor request issued)
///@param	pReq pointer to USB control request structure
///@param	ppAddress address pointer to descriptor in memory
///@param	pDescSize size of the descriptor - by default maxBufferSize
///@return	usbd_acknowledged if the correct descriptor is passed, otherwise usbd_failed
typedef usbd_status_respond (*usbd_descriptor_callback) (usbd_control_request *pReq, void **ppAddress, uint16_t *pDescSize);


///@brief	USB configuration callback function (called if set-configuration request issued)
///@param	pDev pointer to USB device
///@param	configNum configuration number
///@return	usbd_acknowledged if successful
typedef usbd_status_respond (*usbd_config_callback) (usbd_device *pDev, uint8_t configNum);


///@brief	USB device status and capabilities
///@param	none
///@return	hardware status capabilities
typedef uint32_t (*usbd_hw_getInfo) (void);


///@brief	USB enables or disables hardware
///@param	enable USB enable if TRUE, disable if FALSE
///@return	none
typedef void (*usbd_hw_enable) (bool enable);


///@brief	USB connect or disconnect hardware to or from host
///@param	connect USB connect to host in case of TRUE and disconnect otherwise
///@return	connection status
typedef uint8_t (*usbd_hw_connect) (bool connect);


///@brief	USB set hardware address
///@param	address USB address
///@return	none
typedef void (*usbd_hw_setAddress) (uint8_t address);


///@brief	endpoint configuration
///@param	epAddress endpoint address
///@param	epType endpoint type
///@param	epSize endpoint size in byte
///@return	TRUE if configuration was successful
typedef bool (*usbd_hw_ep_config) (uint8_t epAddress, uint8_t epType, uint16_t epSize);


///@brief	endpoint de-configuration
///@param	epAddress endpoint address
///@return	none
typedef void (*usbd_hw_ep_deConfig) (uint8_t ep);


///@brief	read data from OUT endpoint or control endpoint
///@param	epIndex endpoint index
///@param	pBuf pointer to read the buffer
///@param	bufLen length of the read buffer
///@return	length of the received data buffer or -1 in case of error
///@warning	truncates the data if data does not fit in receive buffer
typedef int32_t (*usbd_hw_ep_read) (uint8_t ep, void *pBuf, uint16_t bufLen);


///@brief	writes data to IN endpoint or control endpoint
///@param	epIndex endpoint index
///@param	pDataBuf pointer to data buffer
///@param	bufLen length of the data buffer
///@return	length of the writable data buffer or -1 in case of error
typedef int32_t (*usbd_hw_ep_write) (uint8_t ep, void *pDataBuf, uint16_t bufLen);


///@brief	stall and unstall endpoint
///@param	epAddress endpoint address
///@param	endpoint will be stalled in case of TRUE
///@return	none
///@note	if endpoint is not active, it has also no effect for the endpoint
typedef void (*usbd_hw_ep_setStall) (uint8_t ep, bool stall);


///@brief	checks wheather the endpoint is stalled or not
///@param	epAddress endpoint address
///@return	TRUE if the endpoint is stalled
typedef bool (*usbd_hw_ep_isStalled) (uint8_t ep);


///@brief	USB poll hardware for events
///@param	pDev pointer to the USB device structure
///@param	callback callback to the event which processes subroutines
///@return	none
typedef void (*usbd_hw_poll) (usbd_device *pDev, usbd_event_callback callback);


///@brief	USB get hardware frame number
///@param	none
///@return	USB hardware frame number
typedef uint16_t (*usbd_hw_frameNum) (void);


///@brief	string descriptor with unique serial number form hardware ID
///@param	pBuf pointer to the buffer for the descriptor
///@return	descriptor
typedef uint16_t (*usbd_hw_get_serialNum) (void *pBuf);


///@brief	USB hardware driver call table
struct usbd_driver {
	usbd_hw_getInfo			getInfo;
	usbd_hw_enable			enable;
	usbd_hw_connect			connect;
	usbd_hw_setAddress		setAddress;
	usbd_hw_ep_config		epConfig;
	usbd_hw_ep_deConfig		epDeConfig;
	usbd_hw_ep_read			epRead;
	usbd_hw_ep_write		epWrite;
	usbd_hw_ep_setStall		epSetStall;
	usbd_hw_ep_isStalled	epIsStalled;
	usbd_hw_poll			poll;
	usbd_hw_frameNum		frameNum;
	usbd_hw_get_serialNum	get_serial_num_desc;
};


///@brief	USB device data call table
struct _usbd_device_ {
	const struct usbd_driver		*pDriver;
	usbd_controll_callback			controlCallback;
	usbd_req_complete_callback		reqCompCallback;
	usbd_config_callback			configCallback;
	usbd_descriptor_callback		descriptorCallback;
	usbd_event_callback				events[USBD_EVENT_COUNT];
	usbd_event_callback				endpoint[8];
	usbd_device_status				deviceStatus;
};


///@brief	initialise device structure
///@param	pDev pointer to the USB device which will be initialised
///@param	pDrv pointer to the hardware driver
///@param	ep0size	endpoint 0 size (control endpoint)
///@param	pBuf pointer to control request data buffer
///@param	bufLen size of the data buffer
inline static void USBD_Init(usbd_device *pDev, const struct usbd_driver *pDrv, const uint8_t ep0size, uint32_t *pBuf, const uint16_t bufLen){
	pDev->pDriver = pDrv;
	pDev->deviceStatus.ep0_size = ep0size;
	pDev->deviceStatus.pData = pBuf;
	pDev->deviceStatus.pDataBuffer = pBuf;
	pDev->deviceStatus.data_max_size = bufLen - __builtin_offsetof(usbd_control_request, data);
}


///@brief	USB poll for events
///@param	pDev pointer to device structure
///@return	none
///@note	can be called from main function as well as from USB interrupt
void USBD_Poll(usbd_device *pDev);


///@brief	register callback for all control requests
///@param	pDev pointer to device structure
///@param	callback user control callback
///@return 	none
inline static void USBD_Register_Control(usbd_device *pDev, usbd_controll_callback callback){
	pDev->controlCallback = callback;
}


///@brief	register callback for configuration control requests
///@param	pDev pointer to device structure
///@param	callback pointer to user callback (configuration callback)
///@return	none
inline static void USBD_Register_Config(usbd_device *pDev, usbd_config_callback callback){
	pDev->configCallback = callback;
}


///@brief	register callback for descriptor control requests
///@param	pDev pointer to device structure
///@param	callback pointer to user callback (control callback)
///@return	none
inline static void USBD_Register_Descriptor(usbd_device *pDev, usbd_descriptor_callback callback){
	pDev->descriptorCallback = callback;
}


///@brief	endpoint configuration
///@param	pDev pointer to device structure
///@param	ep endpoint number
///@param	epType type of the endpoint
///@param	epSize	endpoint size
///@return	endpoint configuration
inline static bool USBD_EP_Config(usbd_device *pDev, uint8_t ep, uint8_t epType, uint16_t epSize){
	return pDev->pDriver->epConfig(ep, epType, epSize);
}


///@brief	endpoint deconfiguration
///@param	pDev pointer to device structure
///@param	ep	endpoint number
///@return	none
inline static void USBD_EP_Deconfig(usbd_device *pDev, uint8_t ep){
	pDev->pDriver->epDeConfig(ep);
}


///@brief	register endpoint callback
///@param	pDev pointer to device structur
///@param	ep endpoint number
///@param	callback pointer to user callbac for endpoint events
///@return	none
inline static void USBD_Register_Endpoint(usbd_device *pDev, uint8_t ep, usbd_event_callback callback){
	pDev->endpoint[ep & 0x07] = callback;
}


///@brief	register event callback
///@param	pDev pointer to device structure
///@param	event register the UsB device event
///@param	callback pointer to user for the event
///@return	none
inline static void USBD_Register_Event(usbd_device *pDev, uint8_t event, usbd_event_callback callback){
	pDev->events[event] = callback;
}


///@brief	read from endpoint
///@param	pDev pointer to device structure
///@see		usbd_hw_ep_read
inline static int32_t USBD_EP_Read(usbd_device *pDev, uint8_t ep, void *pBuf, uint16_t bufLen){
	return pDev->pDriver->epRead(ep, pBuf, bufLen);
}


///@brief	write data to endpoint
///@param	pDef pointer to device structure
///@see		usbd_hw_ep_write
inline static int32_t USBD_EP_Write(usbd_device *pDev, uint8_t ep, void* pBuf, uint16_t bufLen){
	return pDev->pDriver->epWrite(ep, pBuf, bufLen);
}


///@brief	stall endpoint
///@param	pDef pointer to device structure
///@param	ep endpoint number
///@return	none
inline static void USBD_EP_Stall(usbd_device *pDev, uint8_t ep){
	pDev->pDriver->epSetStall(ep, 1);
}


///@brief	unstall endpoint
///@param	pDev pointer to device structure
///@param	ep endpoint number
///@return 	none
inline static void USBD_EP_Unstall(usbd_device *pDev, uint8_t ep){
	pDev->pDriver->epSetStall(ep, 0);
}


///@brief	USB enable or disable hardware
///@param	pDev pointer to device structure
///@param	enable USB enable if TURE, disable otherwise
///@return	none
inline static void USBD_Enable(usbd_device *pDev, bool enable){
	pDev->pDriver->enable(enable);
}


///@brief	USB connect or disconnect hardware
///@param	pDev pointer to device structure
///@param	connect USB connect if TURE, disconnect otherwise
///@return	none
inline static uint8_t USBD_Connect(usbd_device *pDev, bool connect){
	return pDev->pDriver->connect(connect);
}


///@brief	retrace status and capabilities
///@param	pDev pointer to device structure
///@return	current hardware status, enumeration speed and capabilities of USB device
inline static uint32_t USBD_GetInfo(usbd_device *pDev){
	return pDev->pDriver->getInfo();
}

#endif


#ifdef __cplusplus
	}
#endif


#endif /* USB_INC_USBD_CORE_H_ */
