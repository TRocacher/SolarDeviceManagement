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
extern int FrontUp;
#define FrontMax (120)



#endif /* INC_MYIT_H_ */
