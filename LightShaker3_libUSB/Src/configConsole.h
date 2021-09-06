/*
 * configConsole.h
 *
 *  Created on: Sep 4, 2016
 *      Author: Christian
 */

#ifndef CONFIGCONSOLE_H_
#define CONFIGCONSOLE_H_

void consoleInit();

//called every 100ms by main-loop if the COM-Port is open
void consoleExecute();

#endif /* CONFIGCONSOLE_H_ */
