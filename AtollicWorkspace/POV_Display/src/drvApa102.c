#include "drvApa102.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_spi.h"
#include "stm32f0xx_rcc.h"


#define LED_CNT		16

colorVrgb leds[LED_CNT];

uint8_t spiSendData[12+4*LED_CNT];

void apa102_init()
{

	//enable clock for periphery modules
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

	//gpio
	GPIO_InitTypeDef GPIO_InitStructure;
	//configure PINs PA7 and PA5 as AF out
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//init the SPI module
	SPI_InitTypeDef SPI_InitStruct;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_Direction = SPI_Direction_Tx;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI1, &SPI_InitStruct);

	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);
	//start the SPI module:
	SPI_Cmd(SPI1,ENABLE);

	DMA_InitTypeDef DMA_InitStruct;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)spiSendData;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(SPI1->DR);
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	DMA_Init(DMA1_Channel3, &DMA_InitStruct);

	apa102_allOff();

}


void updateStripe()
{
	uint8_t ledIdx = 0;
	uint8_t byteIdx = 0;
	DMA_Cmd(DMA1_Channel3, DISABLE);
	//start frame
	for(uint8_t i = 0; i < 4; i++)
	{
		spiSendData[byteIdx] = 0x00;
		byteIdx++;
	}

	for(uint8_t i = 0; i < LED_CNT; i++)
	{
		//led frame
		spiSendData[byteIdx] = 0xE0 |(leds[ledIdx].global & 0x1F); //global
		byteIdx++;
		spiSendData[byteIdx] = leds[ledIdx].blue;
		byteIdx++;
		spiSendData[byteIdx] = leds[ledIdx].green;
		byteIdx++;
		spiSendData[byteIdx] = leds[ledIdx].red;
		byteIdx++;
		ledIdx++;
	}
	//end frame
	//this is not the Frame as stated in the Datasheet! our LEDs (SK9822) seem to be a bit different from the original APA102 here!
	//but it works this way...
	for(uint8_t i = 0; i < 8 ; i++)
	{
		spiSendData[byteIdx] = 0x00;
		byteIdx++;
	}


	//now the DMA has to send the spiSendData to the SPI tx fifo :)
	DMA_SetCurrDataCounter(DMA1_Channel3,12+4*LED_CNT);
	DMA_Cmd(DMA1_Channel3, ENABLE);
}


//so far only one global color

void apa102_setPattern(uint16_t mask, uint8_t global)
{
	//the global-value in the Led-frame is only 5 bit (max.31)!
	if(global > 0x1F)
	{
		global = 0x1F;
	}
	globalColor.global = global;
	for(uint16_t i = 0; i< 16; i++)
	{
		if(mask & (1<<i))
		{	//led shall be active
			leds[i] = globalColor;
		}
		else
		{	//led shall be off
			leds[i].global = 0;
			leds[i].blue = 0;
			leds[i].green = 0;
			leds[i].red = 0;
		}
	}
	updateStripe();
}

void apa102_setSingle(uint8_t index, uint8_t global)
{
	apa102_setPattern(1<<index,global);
}

void apa102_allOff()
{
	for(uint16_t i = 0; i< 16; i++)
	{

		leds[i].global = 0;
		leds[i].blue = 0;
		leds[i].green = 0;
		leds[i].red = 0;

	}
	updateStripe();
}


