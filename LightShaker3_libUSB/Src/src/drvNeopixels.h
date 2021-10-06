#include <main.h>

typedef struct {
	uint8_t global;
	uint8_t blue;
	uint8_t green;
	uint8_t red;
} colorVrgb;

colorVrgb globalColor;
/* farben:
* 0 aus		(0,0,0)
* 1	Blau 	(0,0,1)
* 2	Grün	(0,1,0)
* 3	Zyan	(0,1,1)
* 4	Rot		(1,0,0)
* 5	Magenta	(1,0,1)
* 6	Gelb	(1,1,0)
* 7	Weiss	(1,1,1)
*/
#define COLOR_BLACK		0	//LED off
#define COLOR_BLUE		1
#define COLOR_GREEN		2
#define COLOR_CYAN		3
#define COLOR_RED		4
#define COLOR_MAGENTA	5
#define COLOR_YELLOW	6
#define COLOR_WHITE		7

void Neopixels_init();
void Neopixels_setColor(uint8_t color_idx);
void Neopixels_setColorFullRGB(uint8_t red, uint8_t green, uint8_t blue);
void Neopixels_setPattern(uint16_t mask, uint8_t global);
void Neopixels_Single(uint8_t index, uint8_t global);
void Neopixels_Bargraph(uint8_t hight, uint8_t global, bool group3);
void Neopixels_Off();