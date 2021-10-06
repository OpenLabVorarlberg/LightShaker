/*
 * drvPower.c
 *
 *  Created on: 14.10.2019
 *      Author: ChrLin00
 *      This driver manages the User Button and battery power control
 *      Pins used:
 *      PA1 - Button 	- low if pressed, Vbat if not pressed
 *      PA4 - Supp_Hold	- hold high to keep supply active
 *      PB1 - USB_pres	- high if usb is connected
 *
 *
 */

#include "drvPower.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"

volatile uint8_t power_timer;


uint8_t power_buttonState;

void power_init() {
	//enable clock for periphery modules
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	//init gpios
	//PB1
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	//PA4
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
	//PA1
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	//TODO: implement Vbat measurement
	power_timer = 100; //the system is always on for at least 10 sec
	power_flags = 0;
}

uint8_t power_UsbPresent() {
	return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);
}

/**
 * sets the shutdown-timer to sec seconds
 * after this time, the battery-supply is switched off
 * max time is 25seconds
 * 0 switches off immediatly
 */
void power_hold(uint8_t sec)
{
	if(sec > 25)
	{
		sec = 25;
	}
	power_timer = sec * 10;
}

/**
 * has to be called every 100ms - needed for the power_hold and button detection
 *
 */
void power_exec()
{
//========State-Machine for the Button=============================
	switch(power_buttonState)
	{
	case 0: //released
		//detect button presses
		//the button pulls the pin down, so the logic is inverted!
		if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1))
		{
			power_buttonState = 1;	//change state to bouncing
		}
		break;
	case 1: //bouncing
		//check if pin is still high
		if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1))
		{
			//now the button is surely pressed
			power_flags |= POWER_FLAG_SW_PRESS;
			power_buttonHoldTime = 1;
			power_buttonState = 2;	//change state to pressed / holding
		}
		else
		{
			power_buttonState = 0; //false alarm -> back to released
		}
		break;
	case 2: //pressed
		//count the holding-time
		power_buttonHoldTime++;
		if(power_buttonHoldTime >= 5)
		{
			power_flags |= POWER_FLAG_SW_HOLD;
		}
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1))
		{
			power_flags |= POWER_FLAG_SW_RELEASE;
			power_buttonState = 0;	//pin is low -> button was just released
		}
		break;
	}
//=============================================================================

	//count power_hold time and shut down if exceeded
	if (power_timer)
	{
		power_timer--;
	}
	else
	{
		//shut down the battery supply
		GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);
	}
}
