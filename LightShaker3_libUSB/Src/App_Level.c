/*
 * App_Level.c
 *
 *  Created on: 06.10.2021
 *      Author: ChrLin00
 */


#include <drvAccelerometer.h>
#include <drvNeopixels.h>
#include "App_Level.h"
#include "AppMgmt.h"

int16_t levelResult;
uint8_t led_pos;

void Level_Init()
{
	AppMgmt_Timebase = 10;
	Accelerometer_init();
	Accelerometer_setDataWidth(ACC_DATAWIDTH_8);
	Accelerometer_setRange(RANGE_2G);
	Accelerometer_setDataRate(RATE_12Hz5);
}

void Level_Exec()
{
	levelResult = Accelerometer_read8().y;
	if(levelResult == 0)
	{
		Neopixels_Pattern(0b0000000110000000);
		return;
	}
	if(levelResult < 0)
	{
		led_pos = levelResult + 8;
		//lower limit
		if(led_pos > 250)
		{
			led_pos = 0;
		}
	}
	else
	{	//levelPos > 0
		led_pos = levelResult + 7;
		//upper limit
		if(led_pos > 15)
		{
			led_pos = 15;
		}
	}
	Neopixels_Single(led_pos);
	return;
}
