/*
 * drvDisplay.h
 *
 *  Created on: Aug 30, 2016
 *      Author: Christian
 */

#ifndef DRVDISPLAY_H_
#define DRVDISPLAY_H_




#define ACC_RETURN_TH	100

#define T_FRAME_MIN		500*LOGIC_ROWS		//in usec		//a line needs about 100us to transmit, so they should be min. 500us apart
#define T_FRAME_MAX		65535*32*2	//in usec		//absolute max is (2^16-1)*LOGIC_ROWS*2



volatile uint8_t RowsVisible;
volatile uint8_t RowsOverscan;
volatile uint8_t RowsLogic;						//should be 2^n
volatile uint16_t DispRowMasks[32];




void displayInit();
void displaySendLine();
void displayFindReturnPoint();
void displayEndOfLocktime();

#endif /* DRVDISPLAY_H_ */
