/*
 * drvSwitch.c
 *
 *  Created on: 27.01.2021
 *      Author: AdrMei00
 *      This driver manages the change of the program using the user button
 *      The initialisation of the button is in drvPower.c
 *      Pins used:
 *      PA1 - Button - low if pressed, Vbat if not pressed
 */

#include <drvSwitch.h>
#include "stm32f0xx_rcc.h"
#include "drvProgram.h"
#include "main.h"


void debounce(volatile uint16_t *port, uint16_t pin) {
	uint16_t   port_buffer;
  	uint16_t   debounce_buffer;

  	for(debounce_buffer = 0; debounce_buffer != 0xff;) {
  		debounce_buffer <<= 1;
	  	port_buffer = *port;
	  	delay(150);
	    if((*port & pin) == (port_buffer & pin)) {
	    	debounce_buffer |= 0x01;
	  	}
  	}
}

// change program when battery supply on and user button pressed
// progselect = 0: POV-Display
// progselect = 1: Level
// progselect = 2: Test-pattern
uint8_t switch_exec(uint8_t progselect) {
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) && (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0))
	{
		debounce(GPIOA, GPIO_Pin_4);
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) && (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0))
		{
			switch(progselect)
			{
				case 0: progselect = 1; init_level(); break;
				case 1: progselect = 2; init_test1(); break;
				case 2: progselect = 0; init_povdisplay(); break;
			}
		}
	}
	return progselect;
}
