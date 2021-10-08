/*
 * App_Playground.c
 *
 *  Created on: 06.10.2021
 *      Author: ChrLin00
 */
/**
**===========================================================================
**
**  In this project, you can try some programming yourself.
**  You can use the 16 LEDs and the Sensor for anything you would like to do
**
**===========================================================================
*/

#include <drvAccelerometer.h>
#include <drvNeopixels.h>
#include "App_Playground.h"
#include "drvPower.h"
#include "AppMgmt.h"
#include "main.h"
#include "stdlib.h"

/**
**===========================================================================
**
**  Helper methods for easy access to all features
**
**===========================================================================
*/

// sets the time between to calls of the app_execution (main Loop) in milli-seconds
void SetTimebase(uint16_t ticks)
{
	AppMgmt_Timebase = ticks;
}

/* pauses all code-execution for <ticks> milli-seconds.
 * Be careful with this, as this really blocks the CPU completely
 */
extern void delay (uint32_t ticks);

// liefert eine zufällige zahl im Bereich von 0 bis bereich
// returns a random number between 0 and <range>
uint8_t Random(uint8_t range)
{
	return rand() % (uint16_t)(range+1);
}



uint8_t pos = 0;
int8_t step = 1;
// demo: a red light flowing from left to right and back again
void KnightRider(void)
{
	Neopixels_setColor(COLOR_RED);
	Neopixels_setBrightness(2);
	Neopixels_Single(pos);
	pos += step;
	if(pos >= 15)
	{
		step = -1;
	}
	if(pos == 0)
	{
		step = 1;
	}
}

// demo: turns on and off one LED after the other in random colors
void FarbenSpiel(void)
{
	Neopixels_setColor(Random(6)+1);
	Neopixels_setBrightness(2);
	Neopixels_Single(pos);
	pos ++;
	if(pos > 15)
	{
		pos = 0;
	}
}

/** The following helper functions are available to control the LEDs and write own programs
 *
** Warten(uint32_t ticks) warte die angegeben Zeit bis zum ausführen des nächsten Kommandos
**
** LedEin(uint8_t ledNummer); schalte die LED mit der angegebenen Nummer ein
**
** LedMusterEin(uint16_t muster); schalte alle LEDs ein, bei denen eine "1" steht. Beispiel: LedMusterEin(0b0101010101010101) schaltet jede zweite LED ein.
**
** RGBFarbeAendern(uint8_t rot, uint8_t gruen, uint8_t blau); Ã¤ndere die Farbe fÃ¼r alle LEDs. Beispiel: RGBFarbeAendern(255,0,0) macht alle LEDs rot.
**
** FarbeAendern(uint8_t farbe_palette)
** Farben: 	FARBE_BLAU	FARBE_GRUEN 	FARBE_ZYAN	FARBE_ROT	FARBE_MAGENTA	FARBE_GELB	FARBE_WEISS
**
** AktuelleBeschleunigung(); Gibt den aktuellen Wert des Beschleunigungssensor aus
**
** KnightRider(void); demo: Das rote Leuchtband von KIT aus Knight Rider
**
** FarbenSpiel(void); demo: Schaltet LEDs in zufälligen Farben ein
*/

uint8_t color = COLOR_RED;

/**
**===========================================================================
**
**  Playground_Init: 	You have to add your application's setup code here.
**  					This method is called only once at the very beginning
**
**===========================================================================
*/
void Playground_Init()
{
//-----------------don't change this code-block!------------------
	AppMgmt_Timebase = 100;
	Accelerometer_init();
	Accelerometer_setDataWidth(ACC_DATAWIDTH_8);
	Accelerometer_setDataRate(RATE_400Hz);
//--------------------------------------------------------------

	Neopixels_setColor(COLOR_RED);
	Neopixels_setBrightness(2);
	pos = 0;
	step = 1;
}
/**
**===========================================================================
**
**  Playground_Exec: 	You have to add your application's code here.
**  					This method is called over and over again, until the lightShaker is switched off.
**  					Per Default, the Exec is called 10 times a second (pretty slow)
**  					- this can be changed with the method "SetTimebase()"
**
**===========================================================================
*/
void Playground_Exec()
{
	Neopixels_Single(pos);
	pos += step;
	if(pos >= 15)
	{
		step = -step;
	}
	if(pos == 0)
	{
		step = -step;
	}
}
