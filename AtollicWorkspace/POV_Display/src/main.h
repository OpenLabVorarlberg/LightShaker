#ifndef _main_h
#define _main_h

#include <stddef.h>
#include "stdint.h"
#include "stm32f0xx.h"


void delay(uint32_t ticks);


volatile uint8_t  systemFlags;
#define SYS_FLAG_100MS			(1<<0)

#endif
