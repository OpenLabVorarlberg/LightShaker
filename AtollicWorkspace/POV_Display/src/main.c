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
#include "drvPower.h"
#include "drvNvMemory.h"
#include "drvUSB.h"
#include "drvMMA8653.h"
#include "drvApa102.h"
#include "drvDisplay.h"
#include "stdio.h"
#include "configConsole.h"

/* Private typedef */

/* Private define  */

/* Private macro */

/* Private variables */

/* Private function prototypes */

/* Private functions */

/* Global variables */
volatile uint32_t timer=0;
volatile uint32_t delayTimer;

uint8_t errorcode = 0;

uint8_t tempstring[32];

//systick triggers this every ms
void SysTick_Handler(void)
{
	if(delayTimer)
	{
		delayTimer--;
	}
	timer++;

	if  (timer>=100)
	{
		systemFlags |= SYS_FLAG_100MS;
		timer = 0;
	}
}


void delay(uint32_t ticks)
{
	delayTimer = ticks;
	while(delayTimer > 0);
}


/**
 * NOTES:
 * When measuring the times for a complete frame
 * (time for the movement from turning point to turning point),
 * the minimal t_frame was about 70ms
 *
 * SPI Clock speed = 6MHz
 * a complete line has 576 bits to transmit
 * t_line_transmit = 96us
 * -> max 16x729 pixel theoretically
 * but the leds are only activated at the end of these 96us needed for transmitting, which would make it a bit complicated
 * -> no more than 16x64 pixels!!
 * for full color rgb, the complete frame already takes 3kB!
 */

int main(void)


{
	SystemInit();
	SystemCoreClockUpdate();
	//the systick timer runs with 6MHz
	//config the timer for 1ms interval
	SysTick_Config(SystemCoreClock / 8 / 1000);
	//the systick_config just set the priority of the systick to 15  or so - very low!
	//set it back to 0
	NVIC_SetPriority(SysTick_IRQn,0);
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

	//init the drivers
	power_init();

	apa102_init();

	//LED-Test
	for(uint8_t i = 0; i<16; i++)
	{
		apa102_setSingle(i,10);
		delay(20);

	}
	NvMem_init();
	if(power_UsbPresent())
	{
		//we are attached to a USB-Port!
		USB_Init();
		consoleInit();
	}

	mma8653_init();
	apa102_allOff();
	//TODO: battery-check is still missing

	displayInit();


	while(1)
	{
		if(systemFlags & SYS_FLAG_100MS)
		{
			systemFlags &= ~SYS_FLAG_100MS;
			if(USB_Flags & USB_FLAG_FIRST_CONTACT)
			{
				USB_Flags &= ~USB_FLAG_FIRST_CONTACT;
				USB_VCP_DataTx((uint8_t*)"----LightShaker - POV_Display - Version1.2----\n",0);
			}
			else if(USB_Flags & USB_FLAG_CDC_OPEN)
			{
				if(errorcode != 0)
				{
					sprintf((char*)tempstring,"Error %i occured!\n",errorcode);
					USB_VCP_DataTx((uint8_t*)tempstring,0);
				}
				consoleExecute();
			}

			power_exec();

		}
	}

  return 0;
}
