/*
 * global_defines.h
 *
 *  Created on: Jul 1, 2021
 *      Author: bad_n
 */

#ifndef GLOBAL_DEFINES_H_
#define GLOBAL_DEFINES_H_

#include "stm32f10x.h"

//========== Для инициализации GPIO ======================================================
//---------------inputs-------------------------------------------------
#define INPUT_FLOATING 		0x04 	// вход без подтяжки							0100
#define INPUT_PULL_UP 		0x7F 	// с подтяжкой к питанию
#define INPUT_PULL_DOWN 	0xFF 	// с подтяжкой к "земле"
#define INPUT_ANALOG 		0x00 	// аналоговый вход

#define INPUT_PULL_UP_DOWN 	0x8 	// используется для обоих вариантов				1000
#define GPIO_BITS_MASK 		0xF 	// маска для стирания битов конфигурации		1111

//--------------outputs--------------------------------------------------
#define OUTPUT_OPEN_DRAIN 	0x7 	// выход открытый сток							0111
#define OUTPUT_PUSH_PULL 	0x3 	// выход тяни-толкай							0011

//--------------altarnate function---------------------------------------
#define AF_PUSH_PULL 		0xB 	// альтернативная ф-я с выходом тяни-толкай		1011
#define AF_OPEN_DRAIN 		0xF 	// альтернативная функция с открытым стоком		1111
//========================================================================================


#endif /* GLOBAL_DEFINES_H_ */
