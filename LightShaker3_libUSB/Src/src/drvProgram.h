#include "stdint.h"
#include "drvApa102.h"
#include "drvMMA8653.h"
#include "drvNvMemory.h"
#include "drvPower.h"
#include "drvDisplay.h"
#include "stdio.h"
#include "configConsole.h"
#include "main.h"

#define SYS_FLAG_100MS			(1<<0)

volatile uint8_t  systemFlags;

void init_povdisplay();
void init_level();
void povdisplay();
void level();
void test1();
void init_test1();
