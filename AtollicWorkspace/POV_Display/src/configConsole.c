/*
 * configConsole.c
 *
 *  Created on: Sep 4, 2016
 *      Author: Christian
 */

#include "string.h"
#include "stdio.h"
#include "drvApa102.h"
#include "drvDisplay.h"
#include "eeprom.h"
#include "drvUSB.h"
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
uint16_t VirtAddVarTab[NB_OF_VAR];
#define EE_RED		VirtAddVarTab[0]
#define EE_GREEN	VirtAddVarTab[1]
#define EE_BLUE		VirtAddVarTab[2]
#define EE_ROWS		VirtAddVarTab[3]
#define EE_OVERSCAN	VirtAddVarTab[4]
#define EE_PIC_START 5


uint16_t VarDataTab[NB_OF_VAR];

#define EE_VAL_RED		VarDataTab[0]
#define EE_VAL_GREEN	VarDataTab[1]
#define EE_VAL_BLUE		VarDataTab[2]
#define EE_VAL_ROWS		VarDataTab[3]
#define EE_VAL_OVERSCAN	VarDataTab[4]
#define EE_VAL_PIC_START 5

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
	FLASH_Unlock();

	//fill array with addresses
	for(uint8_t i = 0; i < NB_OF_VAR; i++)
	{
		VirtAddVarTab[i] = (uint16_t)i;
	}

	EE_Init();


	//read values from virtual eeprom (from flash)
	for(uint8_t i = 0; i < NB_OF_VAR; i++)
	{
		EE_ReadVariable(VirtAddVarTab[i],&VarDataTab[i]);
	}
	globalColor.red = EE_VAL_RED;
	globalColor.green = EE_VAL_GREEN;
	globalColor.blue = EE_VAL_BLUE;
	RowsVisible = EE_VAL_ROWS;
	RowsOverscan = EE_VAL_OVERSCAN;
	//uninitialized Flash should read 0xFFFF
	//todo: check that
	if(RowsVisible == 0xFF)
	{
		RowsVisible = 0;
	}

	//the array for the rowData holds 32 values -> limit RowsVisible to that
	if( RowsVisible > 32)
	{
		RowsVisible = 32;
	}
	for(uint8_t i = 0; i< RowsVisible; i++)
	{
		DispRowMasks[i] = VarDataTab[EE_VAL_PIC_START+i];
	}

	//if the device is unconfigured (rowsVisible = 0), set a smiley as default picture
	if(RowsVisible == 0)
	{
		RowsVisible = 16;
		RowsOverscan = 0;
		DispRowMasks[0]  = 0b0000001111000000;
		DispRowMasks[1]  = 0b0000110000110000;
		DispRowMasks[2]  = 0b0001000000001000;
		DispRowMasks[3]  = 0b0010000000000100;
		DispRowMasks[4]  = 0b0100010000010010;
		DispRowMasks[5]  = 0b0100010000010010;
		DispRowMasks[6]  = 0b1000000000001001;
		DispRowMasks[7]  = 0b1000000110001001;
		DispRowMasks[8]  = 0b1000000000001001;
		DispRowMasks[9]  = 0b1000000000001001;
		DispRowMasks[10] = 0b0100010000010010;
		DispRowMasks[11] = 0b0100010000010010;
		DispRowMasks[12] = 0b0010000000000100;
		DispRowMasks[13] = 0b0001000000001000;
		DispRowMasks[14] = 0b0000110000110000;
		DispRowMasks[15] = 0b0000001111000000;
		globalColor.red = 255;
		globalColor.green = 255;
		globalColor.blue = 255;
	}

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
			sprintf((char*)stringBuff,"[%u]\n",RowsVisible);
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
		EE_WriteVariable(EE_ROWS,(uint8_t)rows);
		RowsVisible = (uint8_t) rows;
		sprintf((char*)stringBuff,"saved number of visible rows %u\n",RowsVisible);
		USB_VCP_DataTx((uint8_t*)stringBuff,0);
		consoleStates = CSTATE_ROW_OVERSCAN;
		consoleFlags = CFLAG_NEWSTATE;
		break;
	case CSTATE_ROW_OVERSCAN:	//set number of overscan rows
		if(consoleFlags & CFLAG_NEWSTATE)
		{
			consoleFlags &= ~CFLAG_NEWSTATE;
			USB_VCP_DataTx((uint8_t*)"set number of overscan rows (start and end)",0);
			sprintf((char*)stringBuff,"[%u]\n",RowsOverscan);
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
		EE_WriteVariable(EE_OVERSCAN,(uint8_t)overscan);
		RowsOverscan = (uint8_t)overscan;
		sprintf((char*)stringBuff,"saved number of overscan rows %u\n",RowsVisible);
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
		if(CountOfEnteredDates < RowsVisible)
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
			EE_WriteVariable(VirtAddVarTab[EE_PIC_START+CountOfEnteredDates],rowData);
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
			sprintf((char*)stringBuff,"[%u,%u,%u]\n",EE_VAL_RED,EE_VAL_GREEN,EE_VAL_BLUE);
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
		EE_WriteVariable(EE_RED,(uint8_t)red);
		EE_WriteVariable(EE_GREEN,(uint8_t)green);
		EE_WriteVariable(EE_BLUE,(uint8_t)blue);

		sprintf((char*)stringBuff,"saved global color %u,%u,%u\n",(uint8_t)red,(uint8_t)green,(uint8_t)blue);
		USB_VCP_DataTx((uint8_t*)stringBuff,0);

		consoleStates = CSTATE_FINISHED;
		consoleFlags = CFLAG_NEWSTATE;

		break;
	case CSTATE_FINISHED:
		if(consoleFlags & CFLAG_NEWSTATE)
		{
			consoleFlags &= ~CFLAG_NEWSTATE;
			USB_VCP_DataTx((uint8_t*)"config done, ready for work!\n",0);
		}
		break;
	}
}

