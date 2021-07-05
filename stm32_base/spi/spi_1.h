/*
 * spi_1.h
 *
 *  Created on: Jun 10, 2021
 *      Author: bad_n
 */

#ifndef SPI_SPI_1_H_
#define SPI_SPI_1_H_

//#include "stm32f10x.h"
#include <global_defines.h>


#define pin_SPI1_SCK	5
#define pin_SPI1_MISO	6
#define pin_SPI1_MOSI	7

#define SPI1_ENABLE() 	SET_BIT		(SPI1->CR1, SPI_CR1_SPE)


void SPI1_Init (void);
void SPI1_GPIO_Init (void);
uint8_t SPI1_put_byte (uint8_t tx_byte);


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




#endif /* SPI_SPI_1_H_ */
