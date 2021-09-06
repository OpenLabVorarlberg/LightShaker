/*
 * drvProgram.c
 *
 *  Created on: 27.01.2021
 *      Author: AdrMei00
 *      This file contains the POV-Display, Level and a Test-Pattern
 */

#include "drvProgram.h"

extern uint8_t  timerFlag;

extern uint8_t led_pos;
extern int16_t levelResult;

extern uint8_t errorcode;

extern uint8_t tempstring[32];

void init_povdisplay() {
	apa102_setPattern(0b1000000000000000,2);
	delay(500);

	NvMem_init();
	apa102_allOff();
	//TODO: battery-check is still missing
	displayInit();

	power_hold(10);
}

void init_level() {
	mma8653_setDataWidth(MMA8653_DATAWIDTH_8);
	mma8653_setDataRate(RATE_12Hz5);

	apa102_setPattern(0b1100000000000000,2);
	delay(500);
	apa102_allOff();

	power_hold(10);
}

void init_test1() {
	apa102_setPattern(0b1110000000000000,2);
	delay(500);

	power_hold(10);
}

void povdisplay() {
	if(timerFlag){
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
		power_exec();
		timerFlag = 0;

	}
}

void level() {
	if(timerFlag) {
		power_exec();
		timerFlag = 0;
		levelResult = mma8653_read8().y;
		if(levelResult == 0) {
			apa102_setPattern(0b0000000110000000,2);
		}
		else if(levelResult < 0) {
			led_pos = levelResult + 8;
			//lower limit
			if(led_pos > 250) {
				led_pos = 0;
			}
			apa102_setSingle(led_pos,2);
		}
		else {	//levelPos > 0
			led_pos = levelResult + 7;
			//upper limit
			if(led_pos > 15) {
				led_pos = 15;
			}
			apa102_setSingle(led_pos,2);
		}
	}
}

void test1() {
	if(timerFlag) {
		power_exec();
		timerFlag = 0;
		apa102_setPattern(0b1010101010101010,2);
	}
}
