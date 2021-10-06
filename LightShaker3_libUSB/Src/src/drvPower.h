/*
 * drvPower.h
 *
 *  Created on: 14.10.2019
 *      Author: ChrLin00
 */

#ifndef SRC_DRVPOWER_H_
#define SRC_DRVPOWER_H_

#include "stdint.h"

uint8_t power_buttonHoldTime; //in 1/10s
uint8_t power_flags;
#define POWER_FLAG_SW_RELEASE	(1<<0)
#define POWER_FLAG_SW_PRESS		(1<<1)
#define POWER_FLAG_SW_HOLD		(1<<2)


void power_init();
uint8_t power_UsbPresent();
void power_hold(uint8_t sec);
void power_exec();

#endif /* SRC_DRVPOWER_H_ */
