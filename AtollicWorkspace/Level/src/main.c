/**
*****************************************************************************
**
**  File        : main.c
**
**  Abstract    : main function.
**
**  Functions   : main
**
**  Environment : Atollic TrueSTUDIO(R)
**
**  Distribution: The file is distributed "as is", without any warranty
**                of any kind.
**
**  (c)Copyright Atollic AB.
**  You may use this file as-is or modify it according to the needs of your
**  project. This file may only be built (assembled or compiled and linked)
**  using the Atollic TrueSTUDIO(R) product. The use of this file together
**  with other tools than Atollic TrueSTUDIO(R) is not permitted.
**
*****************************************************************************
*/

/* Includes */
#include "main.h"
#include "stm32f0xx_rcc.h"

#include "drvApa102.h"
#include "drvMMA8653.h"
#include "drvPower.h"

/* Private typedef */

/* Private define  */

/* Private macro */

/* Private variables */

/* Private function prototypes */

/* Private functions */

/* Global variables */
uint32_t timer=0;
uint8_t  timerFlag=0;
volatile uint32_t delayTimer;

/**
**===========================================================================
**
**  Abstract: SysTick interrupt handler
**
**===========================================================================
*/
void SysTick_Handler(void)
{
	if(delayTimer)
	{
		delayTimer--;
	}
	timer++;

	if  (timer>100)
	{
		timerFlag = 1;
		timer = 0;
	}
}


void delay(uint32_t ticks)
{
	delayTimer = ticks;
	while(delayTimer > 0);
}

int main(void)
{
	uint8_t led_pos;
	int16_t levelResult;
	SystemInit();
	SystemCoreClockUpdate();
	//the systick timer runs with 6MHz
	//config the timer for 1ms interval
	SysTick_Config(6000);

	//init the drivers
	power_init();
	apa102_init();
	//LED-Test
	for(uint8_t i = 0; i<16; i++)
	{
		apa102_setSingle(i,10);
		delay(20);
	}
	mma8653_init();
	mma8653_setDataWidth(MMA8653_DATAWIDTH_8);
	mma8653_setDataRate(RATE_12Hz5);

	globalColor.red=0xFF;
	globalColor.green=0xFF;
	globalColor.blue=0xFF;

	apa102_allOff();



	while(1)
	{
		if(timerFlag)
		{
			power_exec();
			timerFlag = 0;
			levelResult = mma8653_read8().y;
			if(levelResult == 0)
			{
				apa102_setPattern(0b0000000110000000,2);
			}
			else if(levelResult < 0)
			{
				led_pos = levelResult + 8;
				//lower limit
				if(led_pos > 250)
				{
					led_pos = 0;
				}
				apa102_setSingle(led_pos,2);
			}
			else	//levelPos > 0
			{
				led_pos = levelResult + 7;
				//upper limit
				if(led_pos > 15)
				{
					led_pos = 15;
				}
				apa102_setSingle(led_pos,2);
			}

		}
	}

  return 0;
}
