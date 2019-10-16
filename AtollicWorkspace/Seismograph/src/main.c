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
#include "stm32f0xx.h"
#include "drvUSB.h"
#include "drvMMA8653.h"
#include "drvApa102.h"
#include "stdio.h"
#include "math.h"
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
uint8_t errorcode = 0;

uint8_t tempstring[32];

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

	if  (timer>5)
	{
		timerFlag = 1;
		timer = 0;
		power_exec();
	}
}


void delay(uint32_t ticks)
{
	delayTimer = ticks;
	while(delayTimer > 0);
}

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{
	SystemInit();
	SystemCoreClockUpdate();
	//the systick timer runs with 6MHz
	//config the timer for 1ms interval
	SysTick_Config(SystemCoreClock / 8 / 1000);
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

	//init the drivers
	power_init();
	power_hold(50);
	if(power_UsbPresent())
	{
		USB_Init();
	}
	mma8653_init();
	apa102_init();

	errorcode = mma8653_setDataWidth(MMA8653_DATAWIDTH_8);


	while(1)
	{
		if(timerFlag)
		{
			timerFlag = 0;


			acc8_t accData = mma8653_read8();
			//calculate the (vectorial) sum of all 3 axes
			uint32_t sum = sqrt((int32_t)(accData.x*accData.x)+(int32_t)(accData.y*accData.y)+(int32_t)(accData.z*accData.z));
			//convert to mg
			//1LSB = 15.6mg
			sum = sum*(double)15.6;
			//display on led-stripe
			uint16_t pattern = 0;
			for(uint8_t i = 0; i < 16; i++)
			{
				if(i<=sum/256)
				{
					pattern |= (1<<i);
				}
				else
				{
					pattern &= ~(1<<i);
				}
			}
			apa102_setPattern(pattern, 2);

			if(USB_Flags & USB_FLAG_FIRST_CONTACT)
			{
				USB_Flags &= ~USB_FLAG_FIRST_CONTACT;
				USB_VCP_DataTx((uint8_t*)"----POV_Seismograph----\n",20);
			}
			if(USB_Flags & USB_FLAG_CDC_OPEN)
			{
				if(errorcode != 0)
				{
					sprintf((char*)tempstring,"Error %i occured!\n",errorcode);
					USB_VCP_DataTx((uint8_t*)tempstring,19);
				}

				sprintf((char*)tempstring,"sum:#1M%i<\n",(int)sum);
				USB_VCP_DataTx((uint8_t*)tempstring,0);

//				sprintf((char*)tempstring,"x:#1M%i<\t",accData.x);
//				USB_VCP_DataTx((uint8_t*)tempstring,5);
//				sprintf((char*)tempstring,"y:#2M%i<\t",accData.y);
//				USB_VCP_DataTx((uint8_t*)tempstring,5);
//				sprintf((char*)tempstring,"z:#3M%i<\t",accData.z);
//				USB_VCP_DataTx((uint8_t*)tempstring,5);
//				USB_VCP_DataTx((uint8_t*)"<\n",2);
			}
		}
	}

  return 0;
}
