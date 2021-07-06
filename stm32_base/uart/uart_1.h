/*
 * uart.h
 *
 *  Created on: Apr 29, 2021
 *      Author: bad_n
 */

#ifndef UART_1_H_
#define UART_1_H_

//#include "stm32f10x.h"
#include <global_defines.h>

volatile uint8_t uart1_fl_rx;

#define	USART1_TX_pin	9
#define	USART1_RX_pin	10

#define	uart1_tx_buf_size  30					//	размер буфера отправки
#define	uart1_rx_buf_size  30 					//	размер буфера приема

//#define	USART1_TX_pin_remaped	6
//#define	USART1_RX_pin_remaped	7

char 	uart1_tx_buf[uart1_tx_buf_size];
char 	uart1_rx_buf[uart1_rx_buf_size];

void uart1_init (uint32_t BaudRate);
	void uart1_port_ini (void);					//	инициализирует пины TX и RX
	void USART1_DMA_init(void);					//	инициализирует контроллер прямого доступа к памяти
		void uart1_dmaTX_init(void);			//	инициализирует передачу из памяти в uart
		void uart1_dmaRX_init(void);			//	инициализирует передачу из uart в память

void put_byte_UART1(uint8_t c);

// нужны статические переменные чтобы работать в прерываннии
volatile  uint8_t uart1_tx_write_index;			//	количество байт оптравленных в очередь (которые уже отправляются)
volatile  uint8_t uart1_tx_counter;				//	количество байт, ожидающих отправку
volatile  uint8_t uart1_DMA_TX_start_position;	//	позиция с которой начнут перенаправлять байты
volatile  uint8_t uart1_DMA_TX_count;			//	сколько байт перенаправить


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



#endif /* UART_1_H_ */










