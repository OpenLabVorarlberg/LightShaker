/*
 * drvPower.h
 *
 *  Created on: 14.10.2019
 *      Author: ChrLin00
 */

#ifndef SRC_DRVPOWER_H_
#define SRC_DRVPOWER_H_

#include "stdint.h"

void power_init();
uint8_t power_UsbPresent();
void power_hold(uint8_t sec);
void power_exec();

#endif /* SRC_DRVPOWER_H_ */
