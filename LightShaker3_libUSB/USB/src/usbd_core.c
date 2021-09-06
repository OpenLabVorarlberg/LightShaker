/*
 * @file			usbd_core.c
 * @brief			functions for the enumeration process of an USB device
 * @author			SteDun00
 * @version			V1.0
 * @review date		2021-02-26
 * @note			with reference to https://github.com/dmitrystu/libusb_stm32/blob/winusb/src/usbd_core.c
 * OMICRON electronics GmbH, Austria
 */


#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "usb.h"


#define _MIN(a,b)	((a) < (b)) ? (a) : (b)


///@brief	USB device process endpoint 0
///@parma	pDev pointer to USB device structure
///@param	event USB device event
///@param	ep USB endpoint number
///@return	none
static void USBD_Process_EP0 (usbd_device *pDev, uint8_t event, uint8_t ep);


///@brief	USB reset device state
///@param	pDev pointer to USB device structure
///@return	none
static void USBD_Process_Reset(usbd_device *pDev){
	pDev->deviceStatus.device_machine_state = usbd_state_default;
	pDev->deviceStatus.control_state = usbd_ctl_idle;
	pDev->deviceStatus.device_configuration = 0;
	pDev->pDriver->epConfig(0, USB_EP_TYPE_CONTROL, pDev->deviceStatus.ep0_size);
	pDev->endpoint[0] = USBD_Process_EP0;
	pDev->pDriver->setAddress(0);
}


///@brief	callback to set USB device address
///@param	pDev pointer to USB device structure
///@param	pReq pointer to USB control request data
///@return	none
static void USBD_Set_Address (usbd_device *pDev, usbd_control_request *pReq){
	pDev->pDriver->setAddress(pReq->wValue);
	if(pReq->wValue) {
		pDev->deviceStatus.device_machine_state = usbd_state_addressed;
	} else {
		pDev->deviceStatus.device_machine_state = usbd_state_default;
	}
}


///@brief	control transfer completion callback processing
///@param	pDev pointer to USB device structure
///@return	none
static void USBD_Process_Callback (usbd_device *pDev){
	if(pDev->reqCompCallback){
		pDev->reqCompCallback(pDev, pDev->deviceStatus.pDataBuffer);
		pDev->reqCompCallback = 0;
	}
}


///@brief	set configuration request processing
///@param	pDev pointer to USB device structure
///@param	config configuration number from request
///@return	usbd_ack if successfuly requested
static usbd_status_respond USBD_Configure (usbd_device *pDev, uint8_t config){
	if(pDev->configCallback){
		if(pDev->configCallback(pDev, config) == usbd_acknowledged){
			pDev->deviceStatus.device_configuration = config;
			if(config){
				pDev->deviceStatus.device_machine_state =  usbd_state_configured;
			} else {
				pDev->deviceStatus.device_machine_state = usbd_state_addressed;
			}
			return usbd_acknowledged;
		}
	}
	return usbd_failed;
}


///@brief	standard control request processing for device
///@param	pDev pointer to USB device structure
///@param	pReq pointer to USB control request
///@return	TRUE if the request is handled
static usbd_status_respond USBD_Process_Device_Request (usbd_device *pDev, usbd_control_request *pReq){
	switch(pReq->bRequest){
	case USB_STANDARD_CLEAR_FEATURE:
		break;
	case USB_STANDARD_GET_CONFIG:
		pReq->data[0] = pDev->deviceStatus.device_configuration;
		return usbd_acknowledged;
	case USB_STANDARD_GET_DESCRIPTOR:
		if(pReq->wValue == ((USB_DESCRIPTOR_TYPE_STRING << 8) | INTERNAL_SERIAL_NO_DESCRIPTOR)){
			pDev->deviceStatus.data_count = pDev->pDriver->get_serial_num_desc(pReq->data);
			return usbd_acknowledged;
		} else {
			if(pDev->descriptorCallback){
				return pDev->descriptorCallback(pReq, &(pDev->deviceStatus.pData), &(pDev->deviceStatus.data_count));
			}
		}
		break;
	case USB_STANDARD_GET_STATUS:
		pReq->data[0] = 0;
		pReq->data[1] = 0;
		return usbd_acknowledged;
	case USB_STANDARD_SET_ADDRESS:
		if(USBD_GetInfo(pDev) & USBD_HW_ADDRESS_BEFORE_STATUS_OUT){
			USBD_Set_Address(pDev, pReq);
		} else {
			pDev->reqCompCallback = USBD_Set_Address;
		}
		return usbd_acknowledged;
	case USB_STANDARD_SET_CONFIG:
		return USBD_Configure(pDev, pReq->wValue);
	case USB_STANDARD_SET_DESCRIPTOR:
		break;
	case USB_STANDARD_SET_FEATURE:
		break;
	default:
		break;
	}
	return usbd_failed;
}


///@brief	standard control request processing for interface
///@param	pDev pointer to USB device structure
///@param	pReq pointer to USB control request
///@return	TRUE if request is handled
static usbd_status_respond USBD_Process_Interface_Request (usbd_device *pDev, usbd_control_request *pReq){
	(void)pDev;
	switch (pReq->bRequest){
	case USB_STANDARD_GET_STATUS:
		pReq->data[0] = 0;
		pReq->data[1] = 0;
		return usbd_acknowledged;
	default:
		break;
	}
	return usbd_failed;
}


///@brief	standard control request processing for endpoint
///@param	pDev pointer to USB device structure
///@param	pReq pointer to USB control request
///@return	TRUE if the request is handled
static usbd_status_respond USBD_Process_EP_Request(usbd_device *pDev, usbd_control_request *pReq){
	switch (pReq->bRequest){
	case USB_STANDARD_SET_FEATURE:
		pDev->pDriver->epSetStall(pReq->wIndex, 1);
		return usbd_acknowledged;
	case USB_STANDARD_CLEAR_FEATURE:
		pDev->pDriver->epSetStall(pReq->wIndex, 0);
		return usbd_acknowledged;
	case USB_STANDARD_GET_STATUS:
		if(pDev->pDriver->epIsStalled(pReq->wIndex)){
			pReq->data[0] = 1;
		} else {
			pReq->data[0] = 0;
		}
		pReq->data[1] = 0;
		return usbd_acknowledged;
	default:
		break;
	}
	return usbd_failed;
}


///@brief	processing control request
///@param	pDev pointer to USB device structure
///@param	pReq pointer to USB control request
///@return	TRUE if request is handled
static usbd_status_respond USBD_Process_Request (usbd_device *pDev, usbd_control_request *pReq){
	if(pDev->controlCallback){
		usbd_status_respond resp = pDev->controlCallback(pDev, pReq, &(pDev->reqCompCallback));
		if(resp != usbd_failed){
			return resp;
		}
	}
	switch (pReq->bmRequestType & (USB_REQ_TYPE_RECIPIENT | USB_REQ_TYPE_TYPE)){
	case USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_DEVICE:
		return USBD_Process_Device_Request(pDev, pReq);
	case USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE:
		return USBD_Process_Interface_Request(pDev, pReq);
	case USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_ENDPOINT:
		return USBD_Process_EP_Request(pDev, pReq);
	default:
		break;
	}
	return usbd_failed;
}


///@brief	control endpoint delay
///@param	pDev pointer to USB device structure
///@param	ep endpoint number
///@return	none
static void USBD_Delay_PID (usbd_device *pDev, uint8_t ep){
	pDev->pDriver->epSetStall(ep & 0x7F, 1);
	pDev->pDriver->epSetStall(ep | 0x80, 1);
	pDev->deviceStatus.control_state = usbd_ctl_idle;
}


///@brief	control endpoint TX event processing
///@param	pDev pointer to USB device structure
///@param	ep endpoint number
///@return	none
static void USBD_Process_EP_Tx (usbd_device *pDev, uint8_t ep){
	int32_t _t = 0;
	switch (pDev->deviceStatus.control_state){
	case usbd_ctl_data_tx_zero_len_packet:
	case usbd_ctl_data_tx:
		_t = _MIN(pDev->deviceStatus.data_count, pDev->deviceStatus.ep0_size);
		pDev->pDriver->epWrite(ep, pDev->deviceStatus.pData, _t);
		pDev->deviceStatus.pData = (uint8_t*)pDev->deviceStatus.pData + _t;
		pDev->deviceStatus.data_count -= _t;
		if(0 != pDev->deviceStatus.data_count){
			break;
		}
		if(usbd_ctl_data_tx == pDev->deviceStatus.control_state || _t != pDev->deviceStatus.ep0_size){
			pDev->deviceStatus.control_state = usbd_ctl_data_last;
		}
		break;
	case usbd_ctl_data_last:
		pDev->deviceStatus.control_state = usbd_ctl_status_out;
		break;
	case usbd_ctl_status_in:
		pDev->deviceStatus.control_state = usbd_ctl_idle;
		USBD_Process_Callback(pDev);
		break;
	default:
		break;
	}
}


///@brief	control endpoint RX event processing
///@param	pDev pointer to USB device structure
///@param	ep endpoint number
///@return 	none
static void USBD_Process_EP_Rx (usbd_device *pDev, uint8_t ep){
	uint16_t _t = 0;
	usbd_control_request *const pReq = pDev->deviceStatus.pDataBuffer;
	switch (pDev->deviceStatus.control_state){
	case usbd_ctl_idle:
		if(0x08 != pDev->pDriver->epRead(ep, pReq, pDev->deviceStatus.data_max_size)){
			USBD_Delay_PID(pDev, ep);
			return;
		}
		pDev->deviceStatus.pData = pReq->data;
		pDev->deviceStatus.data_count = pReq->wLength;
		if((pReq->bmRequestType & USB_REQ_TYPE_DEVTOHOST) || (0 == pReq->wLength)){
			break;
		}
		if(pReq->wLength > pDev->deviceStatus.data_max_size){
			USBD_Delay_PID(pDev, ep);
			return;
		}
		pDev->deviceStatus.control_state = usbd_ctl_data_rx;
		return;
	case usbd_ctl_data_rx:
		_t = pDev->pDriver->epRead(ep, pDev->deviceStatus.pData, pDev->deviceStatus.data_count);
		if(pDev->deviceStatus.data_count < _t){
			USBD_Delay_PID(pDev, ep);
			return;
		} else if (pDev->deviceStatus.data_count != _t){
			pDev->deviceStatus.data_count -= _t;
			pDev->deviceStatus.pData = (uint8_t*)pDev->deviceStatus.pData + _t;
			return;
		}
		break;
	case usbd_ctl_status_out:
		pDev->pDriver->epRead(ep, 0, 0);
		pDev->deviceStatus.control_state = usbd_ctl_idle;
		USBD_Process_Callback(pDev);
		return;
	default:
		USBD_Delay_PID(pDev, ep);
		return;
	}
	pDev->deviceStatus.pData = pReq->data;
	pDev->deviceStatus.data_count = pDev->deviceStatus.data_max_size;
	switch(USBD_Process_Request(pDev, pReq)){
	case usbd_acknowledged:
		if(pReq->bmRequestType & USB_REQ_TYPE_DEVTOHOST){
			if(pDev->deviceStatus.data_count >= pReq->wLength){
				pDev->deviceStatus.data_count = pReq->wLength;
				pDev->deviceStatus.control_state = usbd_ctl_data_tx;
			} else {
				pDev->deviceStatus.control_state = usbd_ctl_data_tx_zero_len_packet;
			}
			USBD_Process_EP_Tx(pDev, ep | 0x80);
		} else {
			pDev->pDriver->epWrite(ep | 0x80, 0, 0);
			pDev->deviceStatus.control_state = usbd_ctl_status_in;
		}
		break;
	case usbd_not_acknowledged:
		pDev->deviceStatus.control_state = usbd_ctl_status_in;
		break;
	default:
		USBD_Delay_PID(pDev, ep);
		break;
	}
}


///@brief	control endpoint 0 event processing callback
///@param	pDev pointer to USB device structure
///@param 	event endpoint event
///@param	ep endpoint number
///@return	none
static void USBD_Process_EP0 (usbd_device *pDev, uint8_t event, uint8_t ep){
	switch (event){
	case USBD_EVENT_PACKET_SETUP:
		pDev->deviceStatus.control_state = usbd_ctl_idle;
		pDev->reqCompCallback = 0;
	case USBD_EVENT_PACKET_RX:
		USBD_Process_EP_Rx(pDev, ep);
		break;
	case USBD_EVENT_PACKET_TX:
		USBD_Process_EP_Tx(pDev, ep);
		break;
	default:
		break;
	}
}


///@brief	general event processing callback
///@param	pDev pointer to USB device structure
///@param	event endpoint event
///@param	ep endpoint number
///@return	none
static void USBD_Process_Event (usbd_device *pDev, uint8_t event, uint8_t ep){
	switch (event){
	case USBD_EVENT_RESET:
		USBD_Process_Reset(pDev);
		break;
	case USBD_EVENT_PACKET_RX:
	case USBD_EVENT_PACKET_TX:
	case USBD_EVENT_PACKET_SETUP:
		if(pDev->endpoint[ep & 0x07]){
			pDev->endpoint[ep & 0x07] (pDev, event, ep);
		}
		break;
	default:
		break;
	}
	if(pDev->events[event]){
		pDev->events[event] (pDev, event, ep);
	}
}


__attribute__((externally_visible)) void USBD_Poll(usbd_device *pDev){
		pDev->pDriver->poll(pDev, USBD_Process_Event);
	}
