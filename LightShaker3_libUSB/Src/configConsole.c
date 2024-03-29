/*
 * configConsole.c
 *
 *  Created on: Sep 4, 2016
 *      Author: Christian
 */

#include <drvNeopixels.h>
#include "string.h"
#include "stdio.h"
#include "drvDisplay.h"
//#include "drvUSB.h"
#include "drvNvMemory.h"
#define CFLAG_NEWSTATE	(1<<0)
#define LENGTH_UNKNOWN	0xFF


enum{
	CSTATE_ROW_VIS,
	CSTATE_ROW_OVERSCAN,
	CSTATE_PICTURE,
	CSTATE_COLOR,
	CSTATE_FINISHED
}consoleStates;

uint8_t consoleFlags;

uint8_t stringBuff[64];

uint8_t CountOfEnteredDates;

/*
 * returns the index of the first occurrence of char in the string starting at startindex
 * if not found, returns 0xFFFF
 * can be used to find the length of an string (charTofind = 0x00, length = LENGTH_UNKNOWN)
 */
uint16_t StrFindChar(uint8_t* string, uint8_t charToFind, uint8_t startIdx, uint8_t length)
{
	for(uint16_t i = startIdx; i < length+startIdx; i++)
	{
		if(string[i] == charToFind)
		{
			return i;
		}
		if(length == LENGTH_UNKNOWN && string[i] == 0x00)
		{
			return 0xFFFF;
		}
	}
	return 0xFFFF;
}

/*
 * string can contain a decimal number, a binary number preceded by "0b"/"0B" or a hex number preceded by "0x"/"0X"
 * the number doesn't have to be right at the beginning, the algorithm searches for the first number (beginning at startIdx)
 * if something goes wrong, it returns 0xFFFFFFFF
 * automatically terminates @ null-character
 */
uint32_t StrToU32(uint8_t* string, uint8_t startIdx, uint8_t length)
{

	uint8_t numberStartIdx = 0;
	uint8_t radix = 2;


	//if the length is not specified, it will be 0xFF -> search for null-character
	if(length == LENGTH_UNKNOWN)
	{
		length = StrFindChar(string, 0x00, startIdx,0xFF)-startIdx;
	}

	//we will need this more than once
	uint8_t endIdx = startIdx + length - 1;


	//search for the first number
	for(uint8_t i = startIdx; i <= endIdx; i++)
	{
		if(string[i] >= '0' && string[i] <= '9')
		{
			//we found the beginning of a number!
			//check if more is coming
			if(i == endIdx)
			{
				//we found a single digit right at the end
				return string[i]-'0';
			}
			//more characters left
			if(string[i+1] >= '0' && string[i+1] <= '9')
			{
				//decimal number
				radix = 10;
				numberStartIdx = i;
				break;
			}
			else if(string[i+1] == 'X' || string[i+1] == 'x')
			{
				//hex
				radix = 16;
				numberStartIdx = i+2;
				break;
			}
			else if(string[i+1] == 'B' || string[i+1] == 'b')
			{
				//hex
				radix = 2;
				numberStartIdx = i+2;
				break;
			}
			else
			{
				//we found a single digit
				return string[i]-'0';
			}
		}
		else
		{
			return 0xFFFFFFFF;
		}
	}
	//we know the number format and the start Index now
	uint32_t result = 0;
	//this loop starts with the first digit!
	for(uint8_t i = 0; i < length-startIdx; i++)
	{
		uint8_t digit = string[numberStartIdx+i];
		switch(radix)
		{
		case 2:
			if(digit == '0' || digit == '1')
			{
				result *= radix;
				result += (digit - '0');
			}
			else
			{
				return result;
			}
			break;
		case 10:
			if(digit >= '0' && digit <= '9')
			{
				result *= radix;
				result += (digit - '0');
			}
			else
			{
				return result;
			}
			break;
		case 16:
			if(digit >= '0' && digit <= '9')
			{
				result *= radix;
				result += (digit - '0');
			}
			else if(digit >= 'a' && digit <= 'f')
			{
				result *= radix;
				result += (digit - 'a'+10);
			}
			else if(digit >= 'A' && digit <= 'F')
			{
				result *= radix;
				result += (digit - 'A'+10);
			}
			else
			{
				return result;
			}
			break;
		}
	}
	return result;
}


void consoleInit()
{
	consoleStates = CSTATE_ROW_VIS;
	consoleFlags |= CFLAG_NEWSTATE;

}

void consoleError()
{
	USB_VCP_DataTx((uint8_t*)"invalid input:",0);
	USB_VCP_DataTx((uint8_t*)stringBuff,0);
	USB_VCP_DataTx((uint8_t*)"\n",0);
	consoleFlags |= CFLAG_NEWSTATE;
	return;
}


void consoleExecute()
{
	uint8_t inputLength = USB_ReadAvailable(stringBuff);
	switch(consoleStates)
	{
	case CSTATE_ROW_VIS:	//set number of visible rows
		if(consoleFlags & CFLAG_NEWSTATE)
		{
			consoleFlags &= ~CFLAG_NEWSTATE;
			USB_VCP_DataTx((uint8_t*)"set number of visible rows - 1..32",0);
			sprintf((char*)stringBuff,"[%u]\n",NvMem_read(NVMEM_AD_ROWS_VISIBLE));
			USB_VCP_DataTx((uint8_t*)stringBuff,0);	//TODO: something very bad happens here!!
		}
		if(inputLength == 0)
		{
			return;
		}
		//the user input could just be a single line break (no change)
		if(stringBuff[0] == '\n')
		{
			USB_VCP_DataTx((uint8_t*)"no change\n",0);
			consoleStates = CSTATE_ROW_OVERSCAN;
			consoleFlags = CFLAG_NEWSTATE;
			return;
		}
		uint32_t rows = StrToU32(stringBuff,0,inputLength);
		if(rows > 32)
		{
			//no number or too big number
			consoleError();
			return;
		}
		NvMem_write(NVMEM_AD_ROWS_VISIBLE,rows);
		sprintf((char*)stringBuff,"saved number of visible rows %u\n",NvMem_read(NVMEM_AD_ROWS_VISIBLE));
		USB_VCP_DataTx((uint8_t*)stringBuff,0);
		consoleStates = CSTATE_ROW_OVERSCAN;
		consoleFlags = CFLAG_NEWSTATE;
		break;
	case CSTATE_ROW_OVERSCAN:	//set number of overscan rows
		if(consoleFlags & CFLAG_NEWSTATE)
		{
			consoleFlags &= ~CFLAG_NEWSTATE;
			USB_VCP_DataTx((uint8_t*)"set number of overscan rows (start and end)",0);
			sprintf((char*)stringBuff,"[%u]\n",NvMem_read(NVMEM_AD_OVERSCAN));
			USB_VCP_DataTx((uint8_t*)stringBuff,0);
			USB_VCP_DataTx((uint8_t*)"the total number of rows (visible + 2*overscan) should be 2^n!\n",0);
		}
		if(inputLength == 0)
		{
			return;
		}
		//the user input could just be a single line break (no change)
		if(stringBuff[0] == '\n')
		{
			USB_VCP_DataTx((uint8_t*)"no change\n",0);
			consoleStates = CSTATE_PICTURE;
			consoleFlags = CFLAG_NEWSTATE;
			return;
		}
		uint32_t overscan = StrToU32(stringBuff,0,inputLength);
		if(overscan > 0xFF)
		{
			//no number or too big number
			consoleError();
			return;
		}
		NvMem_write(NVMEM_AD_OVERSCAN, overscan);
		sprintf((char*)stringBuff,"saved number of overscan rows %u\n",NvMem_read(NVMEM_AD_OVERSCAN));
		USB_VCP_DataTx((uint8_t*)stringBuff,0);
		consoleStates = CSTATE_PICTURE;
		consoleFlags = CFLAG_NEWSTATE;
		break;
	case CSTATE_PICTURE:	//set picture data
		if(consoleFlags & CFLAG_NEWSTATE)
		{
			consoleFlags &= ~CFLAG_NEWSTATE;
			USB_VCP_DataTx((uint8_t*)"enter picture data - one value per row\n",0);
			USB_VCP_DataTx((uint8_t*)"1st row is the leftmost, MSB is top pixel\n",0);
			//Todo: writeback of the saved data?
			CountOfEnteredDates = 0;

			sprintf((char*)stringBuff,"%u>",CountOfEnteredDates);
			USB_VCP_DataTx((uint8_t*)stringBuff,0);
		}
		if(CountOfEnteredDates < NvMem_read(NVMEM_AD_ROWS_VISIBLE))
		{
			if(inputLength == 0)
			{
				return;
			}
			//the user input could just be a single line break

			if(stringBuff[0] == '\n')
			{
				//for row 0, this terminates the complete picture data entry
				if(CountOfEnteredDates == 0)
				{
					USB_VCP_DataTx((uint8_t*)"no change to picture data\n",0);
					consoleStates = CSTATE_COLOR;
					consoleFlags = CFLAG_NEWSTATE;
					return;
				}
			}
			uint32_t rowData = StrToU32(stringBuff,0,inputLength);
			if(rowData > 0xFFFF)
			{
				//no number or too big number
				consoleError();
				return;
			}
			NvMem_write(NVMEM_AD_PICTURE_START+CountOfEnteredDates,rowData);
			USB_VCP_DataTx((uint8_t*)" saved\n",0);
			CountOfEnteredDates++;
			sprintf((char*)stringBuff,"%u>",CountOfEnteredDates);
			USB_VCP_DataTx((uint8_t*)stringBuff,0);

		}
		else
		{
			USB_VCP_DataTx((uint8_t*)"successfully saved new picture\n",0);
			consoleStates = CSTATE_COLOR;
			consoleFlags = CFLAG_NEWSTATE;

		}
		break;
	case CSTATE_COLOR:	//set global color
		if(consoleFlags & CFLAG_NEWSTATE)
		{
			consoleFlags &= ~CFLAG_NEWSTATE;
			USB_VCP_DataTx((uint8_t*)"set global color as uint8_t R, uint8_t G, uint8_t B ",0);
			sprintf((char*)stringBuff,"[%u,%u,%u]\n",NvMem_read(NVMEM_AD_GLOBAL_RED),NvMem_read(NVMEM_AD_GLOBAL_GREEN),NvMem_read(NVMEM_AD_GLOBAL_BLUE));
			USB_VCP_DataTx((uint8_t*)stringBuff,0);
		}
		if(inputLength == 0)
		{
			return;
		}
		//the user input could just be a single line break (no change)
		if(stringBuff[0] == '\n')
		{
			USB_VCP_DataTx((uint8_t*)"no change\n",0);
			consoleStates = CSTATE_FINISHED;
			consoleFlags = CFLAG_NEWSTATE;
			return;
		}
		//search for the 2 needed commas
		uint16_t comma1Idx;
		uint16_t comma2Idx;

		comma1Idx = StrFindChar(stringBuff, ',',1,inputLength);
		if(comma1Idx == 0xFFFF)
		{
			//no comma found -> cannot be parsed
			consoleError();
			return;
		}
		comma2Idx = StrFindChar(stringBuff, ',',comma1Idx + 1,inputLength);
		if(comma1Idx == 0xFFFF)
		{
			//no comma found -> cannot be parsed
			consoleError();
			return;
		}

		//parse the values
		uint32_t red = StrToU32(stringBuff,0,comma1Idx);
		if(red > 0xFF)
		{
			//no number or too big number
			consoleError();
			return;
		}
		uint32_t green = StrToU32(stringBuff,comma1Idx+1,comma2Idx);
		if(green > 0xFF)
		{
			//no number or too big number
			consoleError();
			return;
		}
		uint32_t blue = StrToU32(stringBuff,comma2Idx+1,inputLength);
		if(blue > 0xFF)
		{
			//no number or too big number
			consoleError();
			return;
		}
		//all values ok -> write to flash
		NvMem_write(NVMEM_AD_GLOBAL_RED,red);
		NvMem_write(NVMEM_AD_GLOBAL_GREEN,green);
		NvMem_write(NVMEM_AD_GLOBAL_BLUE,blue);

		sprintf((char*)stringBuff,"saved global color %u,%u,%u\n",(uint8_t)red,(uint8_t)green,(uint8_t)blue);
		USB_VCP_DataTx((uint8_t*)stringBuff,0);

		consoleStates = CSTATE_FINISHED;
		consoleFlags = CFLAG_NEWSTATE;

		break;
	case CSTATE_FINISHED:
		if(consoleFlags & CFLAG_NEWSTATE)
		{
			consoleFlags &= ~CFLAG_NEWSTATE;
			NvMem_SaveToFlash();	//TODO: return errorcode
			USB_VCP_DataTx((uint8_t*)"config done, ready for work!\n",0);
		}
		break;
	}
}


