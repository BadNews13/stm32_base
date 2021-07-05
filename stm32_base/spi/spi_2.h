/*
 * spi_2.h
 *
 *  Created on: Jun 25, 2021
 *      Author: bad_n
 */

#ifndef SPI_2_H_
#define SPI_2_H_

//#include "stm32f10x.h"
#include <global_defines.h>


#define pin_SPI2_SCK	13 	//	(PB13)
#define pin_SPI2_MISO	14	//	(PB14)
#define pin_SPI2_MOSI	15	//	(PB15)

#define SPI2_ENABLE() 	SET_BIT		(SPI2->CR1, SPI_CR1_SPE)


void SPI2_Init (void);
void SPI2_GPIO_Init (void);
uint8_t SPI2_put_byte (uint8_t tx_byte);

//void SPI2_Init_old (void);


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



#endif /* SPI_2_H_ */
