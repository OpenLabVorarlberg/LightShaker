/*
 * App_Level.c
 *
 *  Created on: 06.10.2021
 *      Author: ChrLin00
 */


#include "App_Level.h"
#include "drvApa102.h"
#include "drvMMA8653.h"

int16_t levelResult;
uint8_t led_pos;

void Level_Init()
{
	mma8653_init();
	mma8653_setDataWidth(MMA8653_DATAWIDTH_8);
	mma8653_setDataRate(RATE_12Hz5);
}

void Level_Exec()
{
	levelResult = mma8653_read8().y;
	if(levelResult == 0)
	{
		apa102_setPattern(0b0000000110000000,2);
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
	apa102_setSingle(led_pos,2);
	return;
}
