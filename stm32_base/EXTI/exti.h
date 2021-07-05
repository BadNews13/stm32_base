/*
 * exti.h
 *
 *  Created on: 17 июн. 2021 г.
 *      Author: bad_n
 */

#ifndef EXTI_EXTI_H_
#define EXTI_EXTI_H_

//#include "stm32f10x.h"
#include <global_defines.h>


void EXTI_Init (void);		//	для контроллера (IRQ->PA8)
void EXTI_Init_B1 (void);	//	для отладоной платы


#endif /* EXTI_EXTI_H_ */
