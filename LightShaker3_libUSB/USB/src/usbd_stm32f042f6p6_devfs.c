/*
 * @file			usbd_stm32f042f6p6_devfs.c
 * @brief			USB-stack for the STM32F042F6P6
 * @author			SteDun00
 * @version			V1.0
 * @review date		2021-02-26
 * @note			with reference to https://github.com/dmitrystu/libusb_stm32/blob/winusb/src/usbd_stm32l052_devfs.c
 * OMICRON electronics GmbH, Austria
 */


#include <inttypes.h>
#include <stdbool.h>
#include "usb.h"
#include "stm32f042x6.h"
#include "definitions.h"

#if defined(USBD_STM32L052)

#ifndef USB_PMA_SIZE
	#pragma message "PMA memory size is not defined. Use 1k by default"
	#define USB_PMA_SIZE		0x400
#endif


#define USB_EP_SW_BUF_TX		USB_EP_DTOG_RX
#define USB_EP_SW_BUF_RX		USB_EP_DTOG_TX

#define EP_TOGGLE_SET(epr, bits, mask) *(epr) = (*(epr) ^ (bits)) & (USB_EPREG_MASK | (mask))

#define EP_TX_STALL(epr)    	EP_TOGGLE_SET((epr), USB_EP_TX_STALL, USB_EPTX_STAT)
#define EP_RX_STALL(epr)    	EP_TOGGLE_SET((epr), USB_EP_RX_STALL, USB_EPRX_STAT)
#define EP_TX_UNSTALL(epr)  	EP_TOGGLE_SET((epr), USB_EP_TX_NAK, USB_EPTX_STAT | USB_EP_DTOG_TX)
#define EP_RX_UNSTALL(epr) 		EP_TOGGLE_SET((epr), USB_EP_RX_VALID, USB_EPRX_STAT | USB_EP_DTOG_RX)
#define EP_DTX_UNSTALL(epr) 	EP_TOGGLE_SET((epr), USB_EP_TX_VALID, USB_EPTX_STAT | USB_EP_DTOG_TX | USB_EP_SW_BUF_TX)
#define EP_DRX_UNSTALL(epr) 	EP_TOGGLE_SET((epr), USB_EP_RX_VALID | USB_EP_SW_BUF_RX, USB_EPRX_STAT | USB_EP_DTOG_RX | USB_EP_SW_BUF_RX)
#define EP_TX_VALID(epr)    	EP_TOGGLE_SET((epr), USB_EP_TX_VALID, USB_EPTX_STAT)
#define EP_RX_VALID(epr)    	EP_TOGGLE_SET((epr), USB_EP_RX_VALID, USB_EPRX_STAT)

#define STATUS_VAL(x)   		(USBD_HW_BATTERY_CARGING | (x))


typedef struct _pma_rec_{
	uint16_t address;
	uint16_t count;
} pma_rec;


typedef union _pma_table_ {
	struct {
		pma_rec	tx;
		pma_rec	rx;
	};
	struct {
		pma_rec	tx0;
		pma_rec tx1;
	};
	struct {
		pma_rec	rx0;
		pma_rec	rx1;
	};
} pma_table;


///@brief	helper function for the buffer descriptor table
///@param	ep endpoint number
///@return	pointer to the buffer descriptor table
inline static pma_table *EPT(uint8_t ep){
	return (pma_table*)((ep & 0x07) * 8 + USB_PMAADDR);
}


///@brief	helper function for the endpoint control register
///@param	ep endpoint number
///@return	pointer to the endpoint control register
inline static volatile uint16_t *EPR(uint8_t ep){
	return (uint16_t*)((ep & 0x07) * 4 + USB_BASE);
}


///@brief	helper function to find the next available PMA buffer
///@param	sz
///@return	buffer address for PMA table in uint16_t format
///@note	The PMA buffer grows from top to bottom like a stack.
static uint16_t get_next_pma(uint16_t sz){
	unsigned _result = USB_PMA_SIZE;
	for (uint8_t i = 0; i < 8; i++){
		pma_table *pTbl = EPT(i);
		if((pTbl->rx.address) && (pTbl->rx.address < _result)){
			_result = pTbl->rx.address;
		}
		if((pTbl->tx.address) && (pTbl->tx.address < _result)){
			_result = pTbl->tx.address;
		}
	}
	return (_result < (0x020 + sz)) ? 0 : (_result - sz);
}


///@brief	function to get the information if the USB's device register is enabled
///@param	none
///@return	status value as an unsigned integer value
uint32_t getInfo(void){
	if(!(RCC->APB1ENR & RCC_APB1ENR_USBEN)){
		return STATUS_VAL(0);
	}
	if(USB->BCDR & USB_BCDR_DPPU){
		return STATUS_VAL(USBD_HW_DEVICE_ENABLE | USBD_HW_SPEED_FULL_SPEED);
	}
	return STATUS_VAL(USBD_HW_DEVICE_ENABLE);
}


///@brief	function to set a stall to indicate that the endpoint has halted
///@param	ep endpint number
///@param	stall flag, whether the endpoint is already stalled or not
///@return	none
void ep_setStall(uint8_t ep, bool stall){
	volatile uint16_t *pReg = EPR(ep);
	if(USB_EP_ISOCHRONOUS == (*pReg & USB_EP_T_FIELD)){
		return;
	}
	if (ep & 0x80){
		if(USB_EP_TX_DIS == (*pReg & USB_EPTX_STAT)){
			return;
		}
		if(stall){
			EP_TX_STALL(pReg);
		} else {
			if((USB_EP_KIND | USB_EP_BULK) == (*pReg & (USB_EP_T_FIELD | USB_EP_KIND))){
				EP_DTX_UNSTALL(pReg);
			} else {
				EP_TX_UNSTALL(pReg);
			}
		}
	} else {
		if(USB_EP_RX_DIS == (*pReg & USB_EPRX_STAT)){
			return;
		}
		if(stall){
			EP_RX_STALL(pReg);
		} else {
			if((USB_EP_KIND | USB_EP_BULK) == (*pReg & (USB_EP_T_FIELD | USB_EP_KIND))){
				EP_DRX_UNSTALL(pReg);
			} else {
				EP_RX_UNSTALL(pReg);
			}
		}
	}
}


///@brief	function checks, if the endpoint is stalled or not
///@param	ep endpoint number
///@return	flag, whether the endpoint is stalled or not
bool ep_isStalled(uint8_t ep){
	if(ep & 0x80){
		return (USB_EP_TX_STALL == (USB_EPTX_STAT & *EPR(ep)));
	} else {
		return (USB_EP_RX_STALL == (USB_EPRX_STAT & *EPR(ep)));
	}
}


///@brief		enable the clock for the USB communication and do USB register configurations
///@param		enable function is only executed when the enable flag is true
///@return		none
///@attention	remap pin PA11 and PA12 for USB communication with the STM32f042f6p6
void enable(bool enable) {
	if(enable){
		RCC->APB1ENR |= RCC_APB1ENR_USBEN;
		RCC->APB1RSTR |= RCC_APB1RSTR_USBRST;
		RCC->APB1RSTR &= ~RCC_APB1RSTR_USBRST;

		USB->CNTR = USB_CNTR_CTRM | USB_CNTR_RESETM | USB_CNTR_ERRM |
#if !defined(USBD_SOF_DISABLED)
				USB_CNTR_SOFM |
#endif
				USB_CNTR_SUSPM | USB_CNTR_WKUPM;
	} else if (RCC->APB1ENR & RCC_APB1ENR_USBEN){
		USB->BCDR = 0;
		RCC->APB1RSTR |= RCC_APB1RSTR_USBRST;
		RCC->APB1ENR &= ~RCC_APB1ENR_USBEN;
	}
}


///@brief	function connects the USB device
///@param	connect function is only executed if the connect flag is true
///@return	connection state as integer value
uint8_t connect(bool connect){
	uint8_t res;
	USB->BCDR = USB_BCDR_BCDEN | USB_BCDR_DCDEN;
	if(USB->BCDR & USB_BCDR_DCDET){
		USB->BCDR = USB_BCDR_BCDEN | USB_BCDR_PDEN;
		if(USB->BCDR & USB_BCDR_PS2DET){
			res = USBD_LANE_UNKNOW;
		} else if (USB->BCDR & USB_BCDR_PDET){
			USB->BCDR = USB_BCDR_BCDEN | USB_BCDR_SDEN;
			if(USB->BCDR & USB_BCDR_SDET){
				res = USBD_LANE_CARGING_DEDICATED;
			} else {
				res = USBD_LANE_CARGING_DOWNSTREAM;
			}
		} else {
			res = USBD_LANE_STD_DOWNSTREAM;
		}
	} else {
		res = USBD_LANE_DISCONNECTED;
	}
	USB->BCDR = (connect) ? USB_BCDR_DPPU : 0;
	return res;
}


///@brief	function sets the USB function address, set by the host PC during the enumeration process
///@param	address	address from the host pc as integer value
///@return 	none
void set_address (uint8_t address){
	USB->DADDR = USB_DADDR_EF | address;
}


///@brief	function configures the endpoint
///@param	ep endpoint number
///@param	epType type of the endpoint
///@param	epSize size of the endpoint
///@return	flag, wheather the endpoint configuration was successful or not
bool ep_config (uint8_t ep, uint8_t epType, uint16_t epSize){
	volatile uint16_t *pReg = EPR(ep);
	pma_table *pTbl = EPT(ep);
	if(epSize & 0x01){
		epSize++;
	}

	switch (epType){
	case USB_EP_TYPE_CONTROL:
		*pReg = USB_EP_CONTROL | (ep & 0x07);
		break;
	case USB_EP_TYPE_ISYNCHRONOUS:
		*pReg = USB_EP_ISOCHRONOUS | (ep & 0x07);
		break;
	case USB_EP_TYPE_BULK:
		*pReg = USB_EP_BULK | (ep & 0x07);
		break;
	case USB_EP_TYPE_BULK | USB_EP_TYPE_DBL_BUF:
		*pReg = USB_EP_BULK | USB_EP_KIND | (ep & 0x07);
		break;
	default:
		*pReg = USB_EP_INTERRUPT | (ep & 0x07);
		break;
	}
	if((ep & 0x80) || (epType == USB_EP_TYPE_CONTROL)){
		uint16_t _pma;
		_pma = get_next_pma(epSize);
		if(_pma == 0){
			return false;
		}
		pTbl->tx.address = _pma;
		pTbl->tx.count = 0;
		if((epType == USB_EP_TYPE_ISYNCHRONOUS) || (epType == (USB_EP_TYPE_BULK | USB_EP_TYPE_DBL_BUF))){
			_pma = get_next_pma(epSize);
			if(_pma == 0){
				return false;
			}
			pTbl->tx1.address = _pma;
			pTbl->tx1.count = 0;
			EP_DTX_UNSTALL(pReg);
		} else {
			EP_TX_UNSTALL(pReg);
		}
	}
	if (!(ep & 0x80)){
		uint16_t _rxCount;
		uint16_t _pma;
		if(epSize > 62){
			if(epSize & 0x1F){
				epSize &= 0x1F;
			} else {
				epSize -= 0x20;
			}
			_rxCount = 0x8000 | (epSize << 5);
			epSize += 0x20;
		} else {
			_rxCount = epSize << 9;
		}
		_pma = get_next_pma(epSize);
		if(_pma == 0){
			return false;
		}
		pTbl->rx.address = _pma;
		pTbl->rx.count = _rxCount;
		if((epType == USB_EP_TYPE_ISYNCHRONOUS) || (epType == (USB_EP_TYPE_BULK | USB_EP_TYPE_DBL_BUF))){
			_pma = get_next_pma(epSize);
			if(_pma == 0){
				return false;
			}
			pTbl->rx0.address = _pma;
			pTbl->rx0.count = _rxCount;
			EP_DRX_UNSTALL(pReg);
		} else {
			EP_RX_UNSTALL(pReg);
		}
	}
	return true;
}


///@brief	function de-configures a configured endpoint
///@param	ep endpoint number
///@return	none
void ep_deConfig(uint8_t ep){
	pma_table *pEpt = EPT(ep);
	*EPR(ep) &= ~USB_EPREG_MASK;
	pEpt->rx.address = 0;
	pEpt->rx.count = 0;
	pEpt->tx.address = 0;
	pEpt->tx.count = 0;
}



static uint16_t pma_read(uint8_t *pBuf, uint16_t bufLen, pma_rec *pRx){
	uint16_t *pPma = (void*)(USB_PMAADDR + pRx->address);
	uint16_t rxCount = pRx->count & 0x03FF;
	pRx->count &= ~0x3FF;

	if(bufLen > rxCount){
		bufLen = rxCount;
	}
	rxCount = bufLen;
	while(bufLen){
		uint16_t _t = *pPma;
		*pBuf++ = _t & 0xFF;
		if(--bufLen){
			*pBuf++ = _t >> 8;
			pPma++;
			bufLen--;
		} else {
			break;
		}
	}
	return rxCount;
}



int32_t ep_read (uint8_t ep, void* pBuf, uint16_t bufLen){
	pma_table *pTbl = EPT(ep);
	volatile uint16_t *pReg = EPR(ep);
	switch(*pReg & (USB_EPRX_STAT | USB_EP_T_FIELD | USB_EP_KIND)){
	case (USB_EP_RX_VALID | USB_EP_BULK | USB_EP_KIND):
			switch(*pReg & (USB_EP_DTOG_RX | USB_EP_SW_BUF_RX)){
			case 0:
			case (USB_EP_DTOG_RX | USB_EP_SW_BUF_RX):
				*pReg = (*pReg & USB_EPREG_MASK) | USB_EP_SW_BUF_RX;
			default:
				break;
			}
	if(*pReg & USB_EP_SW_BUF_RX){
		return pma_read(pBuf, bufLen, &(pTbl->rx1));
	} else {
		return pma_read(pBuf, bufLen, &(pTbl->rx0));
	}
	case (USB_EP_RX_VALID | USB_EP_ISOCHRONOUS):
			if(*pReg & USB_EP_DTOG_RX){
				return pma_read(pBuf, bufLen, &(pTbl->rx1));
			} else {
				return pma_read(pBuf, bufLen, &(pTbl->rx0));
			}
	case (USB_EP_RX_NAK | USB_EP_BULK):
	case (USB_EP_RX_NAK | USB_EP_CONTROL):
	case (USB_EP_RX_NAK | USB_EP_INTERRUPT):
	{
		int32_t res = pma_read(pBuf, bufLen, &(pTbl->rx));
		EP_RX_VALID(pReg);
		return res;
	}
	default:
		return -1;
	}
}


static void pma_write (uint8_t *pBuf, uint16_t bufLen, pma_rec *pTx){
	uint16_t *pPma = (void*)(USB_PMAADDR + pTx->address);
	pTx->count = bufLen;

	while(bufLen > 1){
		*pPma++ = pBuf[1] << 8 | pBuf[0];
		pBuf += 2;
		bufLen -= 2;
	}
	if(bufLen){
		*pPma = *pBuf;
	}
}


int32_t ep_write (uint8_t ep, void *pBuf, uint16_t bufLen){
	pma_table *pTbl = EPT(ep);
	volatile uint16_t *pReg = EPR(ep);
	switch(*pReg & (USB_EPTX_STAT | USB_EP_T_FIELD | USB_EP_KIND)){
	case (USB_EP_TX_NAK | USB_EP_BULK | USB_EP_KIND):
			if(*pReg & USB_EP_SW_BUF_TX){
				pma_write(pBuf, bufLen, &(pTbl->tx1));
			} else {
				pma_write(pBuf, bufLen, &(pTbl->tx0));
			}
	*pReg = (*pReg & USB_EPREG_MASK) | USB_EP_SW_BUF_TX;
	break;
	case (USB_EP_TX_VALID | USB_EP_ISOCHRONOUS):
			if(!(*pReg & USB_EP_DTOG_TX)){
				pma_write(pBuf, bufLen, &(pTbl->tx1));
			} else {
				pma_write(pBuf, bufLen, &(pTbl->tx0));
			}
	break;
	case (USB_EP_TX_NAK | USB_EP_BULK):
	case (USB_EP_TX_NAK | USB_EP_CONTROL):
	case (USB_EP_TX_NAK | USB_EP_INTERRUPT):
		pma_write(pBuf, bufLen, &(pTbl->tx));
		EP_TX_VALID(pReg);
		break;
	default:
		return -1;
	}
	return bufLen;
}


uint16_t get_frame (void){
	return USB->FNR & USB_FNR_FN;
}


void event_poll (usbd_device *pDev, usbd_event_callback callback){
	uint8_t _event;
	uint8_t _ep;
	uint16_t _istr = USB->ISTR;
	_ep = _istr & USB_ISTR_EP_ID;
	if(_istr & USB_ISTR_CTR){
		volatile uint16_t *pReg = EPR(_ep);
		if(*pReg & USB_EP_CTR_TX){
			*pReg &= (USB_EPREG_MASK ^ USB_EP_CTR_TX);
			_ep |= 0x80;
			_event = USBD_EVENT_PACKET_TX;
		} else {
			*pReg &= (USB_EPREG_MASK ^ USB_EP_CTR_RX);
			if(*pReg & USB_EP_SETUP){
				_event = USBD_EVENT_PACKET_SETUP;
			} else {
				_event = USBD_EVENT_PACKET_RX;
			}
		}
	} else if (_istr & USB_ISTR_RESET){
		USB->ISTR &= ~USB_ISTR_RESET;
		USB->BTABLE = 0;
		for(uint8_t i = 0; i < 8; i++){
			ep_deConfig(i);
		}
		_event = USBD_EVENT_RESET;
#if !defined(USBD_SOF_DISABLED)
	} else if (_istr & USB_ISTR_SOF){
		_event = USBD_EVENT_SOF;
		USB->ISTR &= ~USB_ISTR_SOF;
#endif
	} else if (_istr & USB_ISTR_WKUP){
		_event = USBD_EVENT_WAKEUP;
		USB->CNTR &= ~USB_CNTR_FSUSP;
		USB->ISTR &= ~USB_ISTR_WKUP;
	} else if (_istr & USB_ISTR_SUSP){
		_event = USBD_EVENT_SUSPEND;
		USB->CNTR |= USB_CNTR_FSUSP;
		USB->ISTR &= ~USB_ISTR_SUSP;
	} else if (_istr & USB_ISTR_ERR){
		USB->ISTR &= ~USB_ISTR_ERR;
		_event = USBD_EVENT_ERROR;
	} else {
		return;
	}
	callback(pDev, _event, _ep);
}


static uint32_t fnv1a32_turn (uint32_t fnv, uint32_t data){
	for (uint8_t i = 0; i < 4; i++){
		fnv ^= (data & 0xFF);
		fnv *= 16777619;
		data >>= 8;
	}
	return fnv;
}


uint16_t get_serial_num_desc (void *pBuffer){
	struct usb_string_descriptor *pDesc = pBuffer;
	uint16_t *pStr = pDesc->wString;
	uint32_t fnv = 2166136261;
	fnv = fnv1a32_turn(fnv, *(uint32_t*)(UID_BASE + 0x00));
	fnv = fnv1a32_turn(fnv, *(uint32_t*)(UID_BASE + 0x04));
	fnv = fnv1a32_turn(fnv, *(uint32_t*)(UID_BASE + 0x14));
	for(uint8_t i = 28; i >= 0; i -= 4){
		uint16_t c = (fnv >> i) & 0x0F;
		c += (c < 10) ? '0' : ('A' - 10);
		*pStr++ = c;
	}
	pDesc->bDescriptorType = USB_DESCRIPTOR_TYPE_STRING;
	pDesc->bLength = 18;
	return 18;
}


__attribute__((externally_visible)) const struct usbd_driver usbd_devfs = {
    getInfo,
    enable,
    connect,
    set_address,
    ep_config,
    ep_deConfig,
    ep_read,
    ep_write,
    ep_setStall,
    ep_isStalled,
    event_poll,
    get_frame,
    get_serial_num_desc,
};

#endif
