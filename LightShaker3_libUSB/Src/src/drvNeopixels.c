/**
 * Driver for a chain of APA102 (Neopixel) adressable RGB-LEDs connected to the SPI
 */

#include <drvNeopixels.h>
#include "stm32f0xx.h"
#include "drvNvMemory.h"
#include "utilHsvToRgb.h"

#define LED_CNT		16
colorVrgb leds[LED_CNT];
uint8_t spiSendData[12 + 4 * LED_CNT];

void Neopixels_init() {

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

	Neopixels_Off();
	Neopixels_SetColor(COLOR_WHITE);
	globalColor.global = 10;

	//LED-Test
	for(uint8_t i = 0; i<16; i++) {
		Neopixels_Single(i);
		Delay(20);
	}
}

void Neopixels_setColorAsSaved()
{
	globalColor.red = NvMem_read(NVMEM_AD_GLOBAL_RED);
	globalColor.green = NvMem_read(NVMEM_AD_GLOBAL_GREEN);
	globalColor.blue = NvMem_read(NVMEM_AD_GLOBAL_BLUE);
}

/**
 * set the color for all leds according to the HSV-Colorformat
 * @param h: hue (position on a color-circle) 0...1535
 * @param s: saturation (how intense the color is. 0 is grey or white) 0...255
 * @param v: value (the brightness - 0 is black or off in case of leds) 0...255
 */
void Neopixels_SetColorHSV(uint16_t h, uint8_t s, uint8_t v)
{
	fast_hsv2rgb_32bit(h, s, v, &globalColor.red, &globalColor.green, &globalColor.blue);
}
void Neopixels_SetColorRGB(uint8_t red, uint8_t green, uint8_t blue) {
	globalColor.red = red;
	globalColor.green = green;
	globalColor.blue = blue;
}

void Neopixels_SetColor(uint8_t color_idx)
{
	globalColor.blue = (color_idx&1)<<7;
	globalColor.green = (color_idx&2)<<6;
	globalColor.red = (color_idx&4)<<5;
}

void Neopixels_SetBrightness(uint8_t b)
{
	//the global-value in the Led-frame is only 5 bit (max.31)!
	if (b > 0x1F) {
		b = 0x1F;
	}
	globalColor.global = b;
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
void Neopixels_Pattern(uint16_t mask) {

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

void Neopixels_Single(uint8_t index) {
	Neopixels_Pattern(1 << index);
}

/**
 * @param group3: 3 lit pixels are grouped together (every 4th is a gap) to increase readability,
 * as humans are very good at counting up to 3, but poor at counting more than 3
 */
void Neopixels_Bargraph(uint8_t hight, bool group3)
{
	uint16_t pattern = 0;
	uint8_t leftouts = 0;
	for(uint8_t ledidx = 0; ledidx < 16; ledidx++)
	{
		if(group3)
		{
			if(ledidx % 4 == 3)
			{
				//every 4th led shall be left out (never be lit and not be counted)
				leftouts++;
				continue;
			}
		}
		if(ledidx-leftouts<hight)
		{
			pattern += 1<<ledidx;
		}

	}
	Neopixels_Pattern(pattern);
	return;
}


void Neopixels_Off() {
	for (uint16_t i = 0; i < 16; i++) {

		leds[i].global = 0;
		leds[i].blue = 0;
		leds[i].green = 0;
		leds[i].red = 0;

	}
	updateStripe();
}

