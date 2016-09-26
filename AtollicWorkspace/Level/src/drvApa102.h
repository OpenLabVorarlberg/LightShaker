#include <stdint.h>

void apa102_init();
void apa102_setPattern(uint16_t mask, uint8_t global);
void apa102_setSingle(uint8_t index, uint8_t global);
void apa102_allOff();
