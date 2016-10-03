/**
**===========================================================================
**
**  In this project, you can try some programming yourself.
**  You can use the 16 LEDs and the Sensor for anything you would like to do
**
**===========================================================================
*/


#include "main.h"
#include "stm32f0xx_rcc.h"
#include "drvApa102.h"
#include "drvMMA8653.h"


//this sets the period (time between two runs) of the application loop in milli-seconds
#define LOOP_TIMEBASE	100


uint32_t timer=0;
uint8_t  timerFlag=0;
volatile uint32_t delayTimer;

/**
**===========================================================================
**
**  setup: 	You have to add your application's setup code here.
**  		This method is called only once at the very beginning
**
**===========================================================================
*/
void setup(void)
{
	//init the drivers
	apa102_init();
	mma8653_init();
	mma8653_setDataWidth(MMA8653_DATAWIDTH_8);
	mma8653_setDataRate(RATE_400Hz);
	globalColor.red=0xFF;
	globalColor.green=0xFF;
	globalColor.blue=0xFF;

	//LED-Test
	for(uint8_t i = 0; i<16; i++)
	{
		apa102_setSingle(i,10);
		delay(20);
	}
	apa102_allOff();
}
/**
**===========================================================================
**
**  loop: 	You have to add your application's code here.
**  		This method is called over and over again, until the lightShaker is switched off.
**  		The macro "LOOP_TIMEBASE" defines how often loop is called every second.
**
**===========================================================================
*/
void loop(void)
{
	uint8_t led_pos;
	int16_t levelResult;

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
/**
**===========================================================================
**
**  don't change the rest of this File!!
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
	if  (timer>=LOOP_TIMEBASE)
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
	SystemInit();
	SystemCoreClockUpdate();
	//the systick timer runs with 6MHz
	//config the timer for 1ms interval
	SysTick_Config(6000);
	setup();

	while(1)
	{
		if(timerFlag)
		{
			timerFlag = 0;
			loop();
		}
	}
  return 0;
}