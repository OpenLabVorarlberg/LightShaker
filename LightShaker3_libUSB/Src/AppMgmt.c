/*
 * AppMgmt.c
 *
 *  Created on: 06.10.2021
 *      Author: ChrLin00
 */


#include <drvNeopixels.h>
#include "AppMgmt.h"
#include "main.h"
#include "App_POVDisplay.h"
#include "App_Level.h"
#include "App_Playground.h"

uint8_t app_active = 0;
bool	app_run = false;
#define APP_COUNT  	3

void AppMgmt_AppInit()
{
	switch(app_active)
	{
		case 0:
			PovD_Init();
			break;
		case 1:
			PovD_DeInit();
			Level_Init();
			break;
		case 2:
			PovD_DeInit();
			Playground_Init();
			break;
	}
	app_run = true;
}

void AppMgmt_AppExec()
{
	if(!app_run)
	{
		return;
	}
	switch(app_active)
	{
	case 0:
		PovD_Exec();
		break;
	case 1:
		Level_Exec();
		break;
	case 2:
		Playground_Exec();
		break;
	}
}


void AppMgmt_CycleApps()
{
	Neopixels_setColor(COLOR_WHITE);
	app_run = false;
	app_active++;
	if(app_active >= APP_COUNT)
	{
		app_active = 0;
	}
	Neopixels_Bargraph(app_active+1, 2, true);
}

void AppMgmt_LoadApp()
{
	Neopixels_Off();
	AppMgmt_AppInit();
}
