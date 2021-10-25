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
extern void Delay (uint32_t ticks);

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
	Neopixels_SetColor(COLOR_RED);
	Neopixels_SetBrightness(2);
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
	Neopixels_SetColor(Random(6)+1);
	Neopixels_SetBrightness(2);
	Neopixels_Single(pos);
	pos ++;
	if(pos > 15)
	{
		pos = 0;
	}
}

/** The following helper functions are available to control the LEDs and write own programs
 *
** Timing
** ------------------------------
** Delay(uint32_t ticks) warte die angegebene Zeit bis zum ausführen des nächsten Kommandos
** SetTimebase(uint16_t ticks) setzt die Zeit in millisekunden, bis die Schleife (Playground_Exec()) wieder aufgerufen wird
**
** LEDs (NeoPixels)
** ------------------------------
** Neopixels_Single(uint8_t index); 				schalte die LED mit der angegebenen Nummer ein (die unterste hat den index 0)
** Neopixels_Pattern(uint16_t mask); 				schalte alle LEDs ein, bei denen eine "1" steht. Beispiel: Neopixels_Pattern(0b0101010101010101) schaltet jede zweite LED ein.
** Neopixels_Bargraph(uint8_t hight, bool group3); 	zeigt ein Streifen aus LEDs mit einstellbarer Höhe (height). Wenn group3 = true, wird jede vierte LED ausgelassen - das macht dem menschlichen Hirn das Zählen viel einfacher
** Neopixels_Off(); 								schaltet alle LEDs aus
**
** Neopixels_SetBrightness(uint8_t b);				setzt die helligkeit aller LEDs auf den wert b (b kann nicht größer als 31 sein)
** Neopixels_SetColorRGB(uint8_t red, uint8_t green, uint8_t blue);
** 													setzt die Farbe anhand der 3 Werte für die Farbkomponenten Rot, Grün und Blau. Beispiel: Neopixels_SetColorRGB(255, 0, 0) ergiebt ein sattes Rot-.
** Neopixels_SetColor(uint8_t color_idx)			setzt die Farbe auf eine der 8 vordefinierten Farben:
** 													COLOR_BLACK (aus)
** 													COLOR_BLUE
** 													COLOR_GREEN
** 													COLOR_CYAN
** 													COLOR_RED
** 													COLOR_MAGENTA
** 													COLOR_YELLOW
** 													COLOR_WHITE
** Neopixels_SetColorHSV(uint16_t h, uint8_t s, uint8_t v);
** 													setzt die Farbe anhand der 3 werte im HSV-Farbraum. Damit kannst du sanfte Farbübergänge machen.
**
** Beschleunigungs-sensor
** ------------------------------
** Accelerometer_read8(); 			Gibt die aktuelle Beschleunigung zurück. Das Ergebnis ist als Vektor in die 3 Achsen x, y und z aufgeteilt.
** 									Beispiel: Accelerometer_read8().x gibt die Beschleunigung in x-richtung als wert von 0 bis 255.
**
** Sonstiges
** ------------------------------
**
** Random(uint8_t range);			Liefert eine zufällige Zahl im Bereich von 0 bis <range>
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

	Neopixels_SetColor(COLOR_RED);
	Neopixels_SetBrightness(2);
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
