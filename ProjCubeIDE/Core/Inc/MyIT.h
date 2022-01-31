/*
 * MyIT.h
 *
 *  Created on: Jan 9, 2022
 *      Author: trocache
 */

#ifndef INC_MYIT_H_
#define INC_MYIT_H_

#include "stm32f1xx_hal.h"

void MyIT_Init(void);

extern char Start;
extern char finish;
extern int FrontUp;
extern uint8_t Tab_Ech[];




#endif /* INC_MYIT_H_ */
