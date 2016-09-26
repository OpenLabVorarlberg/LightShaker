/**
  ******************************************************************************
  * @file    usbd_cdc_vcp.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   Header for usbd_cdc_vcp.c file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CDC_VCP_H
#define __USBD_CDC_VCP_H

#include "usbd_cdc_core.h"
#include  "usbd_usr.h"


#define USB_IT_PRIO                    1 /* USB priority should be lower than USART
                                            in order to not loose data reception from
                                            USART while USB interrupt is being handled*/

volatile uint8_t USB_Flags;
#define USB_FLAG_NEW_DATA				(1<<0)
#define USB_FLAG_CDC_OPEN				(1<<1)
#define USB_FLAG_FIRST_CONTACT			(1<<2)

void USB_Init();
uint16_t USB_VCP_DataTx (uint8_t* Buf, uint32_t Len);
uint8_t USB_ReadByte();
uint32_t USB_ReadAvailable(uint8_t* string);

#endif /* __USBD_CDC_VCP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
