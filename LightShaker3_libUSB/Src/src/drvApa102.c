#include "drvApa102.h"
#include "stm32f0xx.h"
#include "drvNvMemory.h"
#include "main.h"

#define LED_CNT		16
colorVrgb leds[LED_CNT];
uint8_t spiSendData[12 + 4 * LED_CNT];

void apa102_init() {

	//that's what a bare-metal implementation would look like:

	//enable clock for periphery modules
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_DMAEN;
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	//gpio
	//configure PINs PA7 and PA5 as AF out
	GPIOA->MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER7);
	GPIOA->MODER |= GPIO_MODER_MODER5_1 | GPIO_MODER_MODER7_1;
	//configure PINs PA7 and PA5 as high speed out
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5 | GPIO_OSPEEDER_OSPEEDR7;

	//init the SPI module
	//Full-Duplex Master, NSS pin not used, Clock Prescaler 8, MSB first, Cpol 1, CPHA 1
	SPI1->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_BR_1 | SPI_CR1_MSTR
			| SPI_CR1_CPOL | SPI_CR1_CPHA;
	//data size 8b (default in CR2)
	//enable dma request for tx
	SPI1->CR2 = SPI_CR2_TXDMAEN;
	//enable SPI:
	SPI1->CR1 |= SPI_CR1_SPE;

	//config DMA for SPI TX
	//priority high, memToPer, Memory Increment
	DMA1_Channel3->CCR |= DMA_CCR_PL_1 | DMA_CCR_MINC | DMA_CCR_DIR;
	//memory Address
	DMA1_Channel3->CMAR = (uint32_t) spiSendData;
	//peripheral address
	DMA1_Channel3->CPAR = (uint32_t) &(SPI1->DR);

	apa102_allOff();
	globalColor.red = 255;
	globalColor.green = 255;
	globalColor.blue = 255;
	globalColor.global = 10;

	//LED-Test
	for(uint8_t i = 0; i<16; i++) {
		apa102_setSingle(i,10);
		delay(20);
	}
}

void apa102_setGlobalColor(uint8_t red, uint8_t green, uint8_t blue) {
	globalColor.red = red;
	globalColor.green = green;
	globalColor.blue = blue;
}

void updateStripe() {
	uint8_t ledIdx = 0;
	uint8_t byteIdx = 0;

	//disable the DMA channel
	DMA1_Channel3->CCR &= ~DMA_CCR_EN;
	//start frame
	for (uint8_t i = 0; i < 4; i++) {
		spiSendData[byteIdx] = 0x00;
		byteIdx++;
	}

	for (uint8_t i = 0; i < LED_CNT; i++) {
		//led frame
		spiSendData[byteIdx] = 0xE0 | (leds[ledIdx].global & 0x1F); //global
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
	for (uint8_t i = 0; i < 8; i++) {
		spiSendData[byteIdx] = 0x00;
		byteIdx++;
	}

	//now the DMA has to send the spiSendData to the SPI tx fifo :)
	DMA1_Channel3->CNDTR = 12 + 4 * LED_CNT;
	DMA1_Channel3->CCR |= DMA_CCR_EN;
}

//so far only one global color
void apa102_setPattern(uint16_t mask, uint8_t global) {
	//the global-value in the Led-frame is only 5 bit (max.31)!
	if (global > 0x1F) {
		global = 0x1F;
	}
	globalColor.global = global;
	for (uint16_t i = 0; i < 16; i++) {
		if (mask & (1 << i)) {	//led shall be active
			leds[i] = globalColor;
		} else {	//led shall be off
			leds[i].global = 0;
			leds[i].blue = 0;
			leds[i].green = 0;
			leds[i].red = 0;
		}
	}
	updateStripe();
}

void apa102_setSingle(uint8_t index, uint8_t global) {
	apa102_setPattern(1 << index, global);
}

void apa102_Bargraph(uint8_t hight, uint8_t global)
{
	if(!hight)
	{
		apa102_allOff();
		return;
	}
	if(hight > 16)
	{
		hight = 16;
	}
	uint16_t pattern = 0;
	for(uint8_t i = 0; i < hight; i++)
	{
		pattern += 1<<i;
	}
	apa102_setPattern(pattern, global);
	return;
}

void apa102_allOff() {
	for (uint16_t i = 0; i < 16; i++) {

		leds[i].global = 0;
		leds[i].blue = 0;
		leds[i].green = 0;
		leds[i].red = 0;

	}
	updateStripe();
}
