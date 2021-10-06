/*
 * App_POVDisplay.c
 *
 *  Created on: 06.10.2021
 *      Author: ChrLin00
 */

#include <drvAccelerometer.h>
#include <drvNeopixels.h>
#include "App_POVDisplay.h"
#include "drvNvMemory.h"
#include "drvDisplay.h"
#include "drvPower.h"
#include "stm32f0xx_rcc.h"
#include "AppMgmt.h"

/**
 * NOTES POV-Display:
 * When measuring the times for a complete frame
 * (time for the movement from turning point to turning point),
 * the minimal t_frame was about 70ms
 *
 * SPI Clock speed = 6MHz
 * a complete line has 576 bits to transmit
 * t_line_transmit = 96us
 * -> max 16x729 pixel theoretically
 * but the leds are only activated at the end of these 96us needed for transmitting, which would make it a bit complicated
 * -> no more than 16x64 pixels!!
 * for full color rgb, the complete frame already takes 3kB!
 */

void PovD_Init()
{
	AppMgmt_Timebase = 1;
	NvMem_init();
	Accelerometer_init();
	Neopixels_Off();
	displayInit();

	power_hold(10);
}

void PovD_Exec()
{
	//the Display is completely interrupt-controlled

	/*if(USB_Flags & USB_FLAG_FIRST_CONTACT) {
		USB_Flags &= ~USB_FLAG_FIRST_CONTACT;
		USB_VCP_DataTx((uint8_t*)"----LightShaker - POV_Display - Version1.2----\n",0);
	}
	else if(USB_Flags & USB_FLAG_CDC_OPEN) {
		if(errorcode != 0) {
			sprintf((char*)tempstring,"Error %i occured!\n",errorcode);
			USB_VCP_DataTx((uint8_t*)tempstring,0);
		}
		consoleExecute();
	}*/
}

void PovD_DeInit()
{
	Accelerometer_deInit();
	NVIC_DisableIRQ(TIM2_IRQn);
	NVIC_ClearPendingIRQ(TIM2_IRQn);
	NVIC_DisableIRQ(TIM3_IRQn);
	NVIC_ClearPendingIRQ(TIM3_IRQn);

}
