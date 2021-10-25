/*
 * App_ColorPicker.c
 *
 *  Created on: 07.10.2021
 *      Author: ChrLin00
 */

#include "App_ColorPicker.h"
#include "drvAccelerometer.h"
#include "drvNeopixels.h"
#include "drvPower.h"
#include "AppMgmt.h"
#include "drvNvMemory.h"

uint16_t h = 0;
uint8_t s = 128;

void ColorPicker_Init()
{
	AppMgmt_Timebase = 10;
	Accelerometer_init();
	Accelerometer_setDataWidth(ACC_DATAWIDTH_8);
	Accelerometer_setDataRate(RATE_400Hz);
	Accelerometer_setRange(RANGE_2G);
	Neopixels_SetBrightness(2);
	NvMem_init();
	power_hold(20);
	power_flags &= ~ POWER_FLAG_SW_DOUBLE;
}


void ColorPicker_Exec()
{
	int8_t dh = Accelerometer_read8().x>>2;
	if(dh < 0)
	{
		if(h >= dh*-1)
		{
			h += dh;
		}
		else
		{
			//it doesn't fit-> wrap araund
			h = h +dh+HSV_H_MAX;
		}
	}
	if(dh > 0)
	{
		h = (h + dh) % HSV_H_MAX;
	}

	int8_t ds = (Accelerometer_read8().y>>4)*-1;
	if(ds < 0)
	{
		if(s >= ds*-1)
		{
			s += ds;
		}
		else
		{
			s = 0;
		}
	}
	if(ds > 0)
	{
		if(255-s > ds)
		{
			s += ds;
		}
		else
		{
			s = 255;
		}
	}
	Neopixels_SetColorHSV(h, s, 255);
	Neopixels_Pattern(0xFFFF);

	//a double-click saves the current color to NvMemory
	if(power_flags & POWER_FLAG_SW_DOUBLE)
	{
		power_flags &= ~POWER_FLAG_SW_DOUBLE;
		power_hold(2);
		Neopixels_Off();
		NvMem_write(NVMEM_AD_GLOBAL_RED, globalColor.red);
		NvMem_write(NVMEM_AD_GLOBAL_GREEN, globalColor.green);
		NvMem_write(NVMEM_AD_GLOBAL_BLUE, globalColor.blue);
		Neopixels_SetBrightness(6);
		if(!NvMem_SaveToFlash())
		{
			Neopixels_SetColor(COLOR_GREEN);
		}
		else
		{
			Neopixels_SetColor(COLOR_RED);
		}
		Neopixels_Single(0);
		Delay(200);
		Neopixels_Off();
	}
}
