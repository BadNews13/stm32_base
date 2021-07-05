/*
 * uart.h
 *
 *  Created on: Apr 29, 2021
 *      Author: bad_n
 */

#ifndef UART_3_H_
#define UART_3_H_

//#include "stm32f10x.h"
#include <global_defines.h>

#define	USART3_TX_pin	10
#define	USART3_RX_pin	11

#define	uart3_tx_buf_size  30					//	размер буфера отправки
#define	uart3_rx_buf_size  30 					//	размер буфера приема

char 	uart3_tx_buf[uart3_tx_buf_size];
char 	uart3_rx_buf[uart3_rx_buf_size];

void uart3_init (uint32_t BaudRate);
	void uart3_port_ini (void);					//	инициализирует пины TX и RX
	void USART3_DMA_init(void);					//	инициализирует контроллер прямого доступа к памяти
		void uart3_dmaTX_init(void);			//	инициализирует передачу из памяти в uart
		void uart3_dmaRX_init(void);			//	инициализирует передачу из uart в память

void put_byte_UART3(uint8_t c);

// нужны статические переменные чтобы работать в прерываннии
volatile  uint8_t uart3_tx_write_index;			//	количество байт оптравленных в очередь (которые уже отправляются)
volatile  uint8_t uart3_tx_counter;				//	количество байт, ожидающих отправку
volatile  uint8_t uart3_DMA_TX_start_position;	//	позиция с которой начнут перенаправлять байты
volatile  uint8_t uart3_DMA_TX_count;			//	сколько байт перенаправить


//========== Для инициализации GPIO ======================================================
//---------------inputs-------------------------------------------------
#define INPUT_FLOATING 		0x04 	// вход без подтяжки							0100
#define INPUT_PULL_UP 		0x7F 	// с подтяжкой к питанию
#define INPUT_PULL_DOWN 	0xFF 	// с подтяжкой к "земле"
#define INPUT_ANALOG 		0x00 	// аналоговый вход

#define INPUT_PULL_UP_DOWN 	0x08 	// используется для обоих вариантов
#define GPIO_BITS_MASK 		0x0F 	// маска для стирания битов конфигурации

//--------------outputs--------------------------------------------------
#define OUTPUT_OPEN_DRAIN 	0x07 	// выход открытый сток
#define OUTPUT_PUSH_PULL 	0x03 	// выход тяни-толкай

//--------------altarnate function---------------------------------------
#define AF_PUSH_PULL 		0x0B 	// альтернативная ф-я с выходом тяни-толкай		1011
#define AF_OPEN_DRAIN 		0x0F 	// альтернативная функция с открытым стоком		1111
//========================================================================================



#endif /* UART_3_H_ */










