#include "stdint.h"
#include "stm32f0xx_gpio.h"

void debounce(volatile uint16_t *port, uint16_t pin);
uint8_t switch_exec(uint8_t progselect);
