#include <stdint.h>

typedef struct {
	uint8_t global;
	uint8_t blue;
	uint8_t green;
	uint8_t red;
} colorVrgb;

colorVrgb globalColor;

void apa102_init();
void apa102_setGlobalColor(uint8_t red, uint8_t green, uint8_t blue);
void apa102_setPattern(uint16_t mask, uint8_t global);
void apa102_setSingle(uint8_t index, uint8_t global);
void apa102_Bargraph(uint8_t hight, uint8_t global);
void apa102_allOff();
