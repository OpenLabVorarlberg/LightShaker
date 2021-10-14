/*
 * drvNvMemory.c
 *
 *  Created on: 15.10.2019
 *      Author: ChrLin00
 *
 *      alternative to the crap eeprom emulator from st
 *      Writing to flash is not protected! (if Power is removed during write, the user data in flash is corruted!)
 *      When supporting a multicolor display, we need up to 3kB of Flash to save the Picture-Data -> this needs to support more than 1 Page!
 *      Only 16-bit read/write and addressing
 *      as Flash always initializes with 0xFF, we save everything inverted. That way we get a 0 as default
 */
#include "drvNvMemory.h"
#include "stm32f0xx.h"

#define NVMEM_PAGE_COUNT		1	//if you change that, don't forget to change the length of the flash-section in the linker-script!
#define NVMEM_FLASH_PAGESIZE	1024
#define NVMEM_FLASH_BASE		(0x08008000 - NVMEM_PAGE_COUNT*NVMEM_FLASH_PAGESIZE)
#define NVMEM_AD_TOP			(NVMEM_FLASH_PAGESIZE*NVMEM_PAGE_COUNT/2)-1


#define READ16(address) ((uint16_t)(*(uint16_t*)(address)))

uint16_t NvMem_FlashData[NVMEM_WORDS_ALLOCATED] __attribute__ ((section (".nvmem")))=
{
		~0x00FF,				//NVMEM_AD_GLOBAL_RED
		~0x00FF,				//NVMEM_AD_GLOBAL_GREEN
		~0x00FF,				//NVMEM_AD_GLOBAL_BLUE
		~0x0010,				//NVMEM_AD_ROWS_VISIBLE
		~0x0000,				//NVMEM_AD_OVERSCAN
		~0b0000001111000000,	//start of default picture (smiley, 16x16 pixel monochrome)
		~0b0000110000110000,
		~0b0001000000001000,
		~0b0010000000000100,
		~0b0100010000010010,
		~0b0100010000010010,
		~0b1000000000001001,
		~0b1000000110001001,
		~0b1000000000001001,
		~0b1000000000001001,
		~0b0100010000010010,
		~0b0100010000010010,
		~0b0010000000000100,
		~0b0001000000001000,
		~0b0000110000110000,
		~0b0000001111000000 //end of picture (offset=21)
};

uint16_t NvMem_RamBuffer[NVMEM_WORDS_ALLOCATED];
bool NvMem_Writable;

/**
 * loads user data from flash into memory for easy access and change in the application
 */
void NvMem_init() {

	//enable flash-if clock
	RCC->AHBENR |= RCC_AHBENR_FLITFEN;

	for (uint16_t ad = 0; ad < NVMEM_WORDS_ALLOCATED; ad++) {
		//NvMem_RamBuffer[ad] = ~READ16(NVMEM_FLASH_BASE+2*ad);
		NvMem_RamBuffer[ad] = ~NvMem_FlashData[ad];
	}
	NvMem_Writable = true;
}

/**
 * Reads 16 bit form the user space in flash
 * address: 16-bit address (512 addresses per used flash page)
 */
uint16_t NvMem_read(uint16_t address) {
	if (address > NVMEM_AD_TOP) {
		return ERR_NVMEM_OUTOFRANGE;
	}
	return NvMem_RamBuffer[address];
}

/**
 * Writes 16 bit to the user space in flash
 * address: 16-bit address (512 addresses per used flash page)
 * data: data to write
 * returns: errorcode
 */
uint8_t NvMem_write(uint16_t address, uint16_t data) {
	if (address > NVMEM_AD_TOP) {
		return ERR_NVMEM_OUTOFRANGE;
	}
	if (!NvMem_Writable) {
		return ERR_NVMEM_ALREADYWRITTEN;
	}
	NvMem_RamBuffer[address] = data;
	return 0;
}

/**
 * This is the only place we're writing to flash
 * This can only be done once for every start of the device!
 * The erasing and writing takes some time, during this time the device is not responsive and must not be unpowered!
 */
uint8_t NvMem_SaveToFlash() {

	//wait till flash is ready
	while (FLASH->SR & FLASH_SR_BSY) {

	}
	//unlock flash
	if ((FLASH->CR & FLASH_CR_LOCK) != 0) {
		FLASH->KEYR = FLASH_FKEY1;
		FLASH->KEYR = FLASH_FKEY2;
	}
	//erase all pages assigned to the nvmem
	FLASH->CR |= FLASH_CR_PER;
	for (uint8_t i = 0; i < NVMEM_PAGE_COUNT; i++) {
		FLASH->AR = NVMEM_FLASH_BASE + i * NVMEM_FLASH_PAGESIZE;
		FLASH->CR |= FLASH_CR_STRT;
		while ((FLASH->SR & FLASH_SR_BSY) != 0) {

		}
		if ((FLASH->SR & FLASH_SR_EOP) != 0) {
			FLASH->SR = FLASH_SR_EOP;
		} else {
			return ERR_FLASH_ERASEFAIL;
		}
	}
	FLASH->CR &= ~FLASH_CR_PER;
	//write data
	while ((FLASH->SR & FLASH_SR_BSY) != 0) {

	}
	uint32_t flash_ad = NVMEM_FLASH_BASE;
	for (uint16_t ad = 0; ad < NVMEM_AD_TOP; ad++) {
		FLASH->CR |= FLASH_CR_PG;
		uint16_t value_to_write = ~NvMem_RamBuffer[ad];
		*(uint16_t*) (flash_ad) = value_to_write;
		while ((FLASH->SR & FLASH_SR_BSY) != 0) {

		}
		if (FLASH->SR & FLASH_SR_PGERR) {
			return ERR_FLASH_NOTERASED;
		}
		if (FLASH->SR & FLASH_SR_WRPRTERR) {
			return ERR_FLASH_WRPTERR;
		}
		uint16_t readback = READ16(flash_ad);
		if (readback != (uint16_t) ~(NvMem_RamBuffer[ad])) {
			return ERR_FLASH_WRONGREADBACK;
		}
		flash_ad += 2;
	}
	NvMem_Writable = false;
	return 0;
}

