#include "dbgLED.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"

void dbgLED_init()
{
#ifdef	DBG_LED_ENABLED
	//clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	//gpio
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
}

void dbgLED_on()
{
#ifdef	DBG_LED_ENABLED
	GPIO_WriteBit(GPIOA,GPIO_Pin_1,Bit_SET);
#endif
}
void dbgLED_off()
{
#ifdef	DBG_LED_ENABLED
	GPIO_WriteBit(GPIOA,GPIO_Pin_1,Bit_RESET);
#endif
}
void dbgLED_toggle()
{
	if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_1))
	{
		GPIO_WriteBit(GPIOA,GPIO_Pin_1,Bit_RESET);
	}
	else
	{
		GPIO_WriteBit(GPIOA,GPIO_Pin_1,Bit_SET);
	}
}