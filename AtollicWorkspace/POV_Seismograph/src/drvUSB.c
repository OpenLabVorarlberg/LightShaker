/**
  ******************************************************************************
  * @file    usbd_cdc_vcp.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   Generic media access Layer.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "drvUSB.h"
#include "utilFifo.h"
#include "stm32f0xx_rcc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USB_CORE_HANDLE  USB_Device_dev ;

/* These are external variables imported from CDC core to be used for IN 
   transfer management. */
extern uint8_t  APP_Rx_Buffer []; /* Write CDC received data in this buffer.
                                     These data will be sent over USB IN endpoint
                                     in the CDC core functions. */
extern uint32_t APP_Rx_ptr_in;    /* Increment this pointer or roll it back to
                                     start address when writing received data
                                     in the buffer APP_Rx_Buffer. */

//buffer for data coming from the PC
FifoBuffer UsbPcSendBuf;



/* Private function prototypes -----------------------------------------------*/
uint16_t USB_VCP_Init     (void);
uint16_t USB_VCP_DeInit   (void);
uint16_t USB_VCP_Ctrl     (uint32_t Cmd, uint8_t* Buf, uint32_t Len);
uint16_t USB_VCP_DataTx   (uint8_t* Buf, uint32_t Len);
uint16_t USB_VCP_DataRx   (uint8_t* Buf, uint32_t Len);


CDC_IF_Prop_TypeDef VCP_fops =
{
		USB_VCP_Init,
		USB_VCP_DeInit,
		USB_VCP_Ctrl,
		USB_VCP_DataTx,
		USB_VCP_DataRx
};

/* Private functions ---------------------------------------------------------*/

void USB_Init()
{


	//remap Pins PA11/12
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG->CFGR1 |= SYSCFG_CFGR1_PA11_PA12_RMP;

	USBD_Init(&USB_Device_dev,
				&USR_desc,
				&USBD_CDC_cb,
				&USR_cb);
	USB_Flags &= ~USB_FLAG_NEW_DATA;
	UsbPcSendBuf.read = 0;
	UsbPcSendBuf.write = 0;
}

/**
  * @brief  VCP_Init
  *         Initializes the Media on the STM32
  * @param  None
  * @retval Result of the operation (USBD_OK in all cases)
  */
uint16_t USB_VCP_Init(void)
{

  return USBD_OK;
}

/**
  * @brief  VCP_DeInit
  *         DeInitializes the Media on the STM32
  * @param  None
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
uint16_t USB_VCP_DeInit(void)
{

  return USBD_OK;
}


/**
  * @brief  VCP_Ctrl
  *         Manage the CDC class requests
  * @param  Cmd: Command code            
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the operation (USBD_OK in all cases)
  */
uint16_t USB_VCP_Ctrl (uint32_t Cmd, uint8_t* Buf, uint32_t Len)
{ 

	switch(Cmd)
	{
	//if we receive this command, we know that a terminal connection is being opened!
	case SET_LINE_CODING:
		if(!(USB_Flags & USB_FLAG_CDC_OPEN))
		{
			USB_Flags |= USB_FLAG_CDC_OPEN;
			USB_Flags |= USB_FLAG_FIRST_CONTACT;
		}
		break;
	}
	return USBD_OK;
}

/**
  * @brief  VCP_DataTx
  *         CDC received data to be send over USB IN endpoint are managed in
  *         this function.
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else VCP_FAIL
  */
uint16_t USB_VCP_DataTx (uint8_t* Buf, uint32_t Len)
{
	if(Len)
	{
		for(uint32_t i = 0; i < Len; i++)
		{
			APP_Rx_Buffer[APP_Rx_ptr_in] = Buf[i];
			APP_Rx_ptr_in++;

			/* To avoid buffer overflow */
			if(APP_Rx_ptr_in == APP_RX_DATA_SIZE)
			{
				APP_Rx_ptr_in = 0;
			}
		}
	}
	else
	//stop @ null character (Len = 0)
	{
		uint32_t i = 0;
		while(Buf[i] != 0x00)
		{
			APP_Rx_Buffer[APP_Rx_ptr_in] = Buf[i];
			APP_Rx_ptr_in++;

			/* To avoid buffer overflow */
			if(APP_Rx_ptr_in == APP_RX_DATA_SIZE)
			{
				APP_Rx_ptr_in = 0;
			}
			i++;
		}
	}
	return USBD_OK;
}

/**
  * @brief  VCP_DataRx
  *         Data received over USB OUT endpoint are sent over CDC interface 
  *         through this function.
  * @note   This function will block any OUT packet reception on USB endpoint 
  *         untill exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result 
  *         in receiving more data while previous ones are still not sent.
  *                 
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else VCP_FAIL
  */
uint16_t USB_VCP_DataRx (uint8_t* Buf, uint32_t Len)
{
	USB_Flags |= USB_FLAG_NEW_DATA;
	for(uint32_t i = 0; i < Len; i++)
	{
		FifoIn(&UsbPcSendBuf, Buf[i]);
	}
	return USBD_OK;
}

/**
 * @retval returns null if the buffer is empty
 */
uint8_t USB_ReadByte()
{
	USB_Flags &= ~USB_FLAG_NEW_DATA;
	uint8_t tempByte = 0;
	if(FifoOut(&UsbPcSendBuf, &tempByte) == FIFO_FAIL)
	{
		return (char)0x00;
	}
	return (char)tempByte;
}

uint32_t USB_ReadAvailable(uint8_t* string)
{
	USB_Flags &= ~USB_FLAG_NEW_DATA;
	uint32_t count = FifoCount(&UsbPcSendBuf);
	for (uint32_t i = 0; i < count; i++)
	{
		FifoOut(&UsbPcSendBuf,(uint8_t*)string+i);
	}
	return count;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
