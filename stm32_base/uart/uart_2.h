/*
 * uart.h
 *
 *  Created on: Apr 29, 2021
 *      Author: bad_n
 */

#ifndef UART_2_H_
#define UART_2_H_

//#include "stm32f10x.h"
#include <global_defines.h>

#define	USART2_TX_pin	2
#define	USART2_RX_pin	3

#define	uart2_tx_buf_size  30					//	размер буфера отправки
#define	uart2_rx_buf_size  30 					//	размер буфера приема

char 	uart2_tx_buf[uart2_tx_buf_size];
char 	uart2_rx_buf[uart2_rx_buf_size];

void uart2_init (uint32_t BaudRate);
	void uart2_port_ini (void);					//	инициализирует пины TX и RX
	void USART2_DMA_init(void);					//	инициализирует контроллер прямого доступа к памяти
		void uart2_dmaTX_init(void);			//	инициализирует передачу из памяти в uart
		void uart2_dmaRX_init(void);			//	инициализирует передачу из uart в память

void put_byte_UART2(uint8_t c);

// нужны статические переменные чтобы работать в прерываннии
volatile  uint8_t uart2_tx_write_index;			//	количество байт оптравленных в очередь (которые уже отправляются)
volatile  uint8_t uart2_tx_counter;				//	количество байт, ожидающих отправку
volatile  uint8_t uart2_DMA_TX_start_position;	//	позиция с которой начнут перенаправлять байты
volatile  uint8_t uart2_DMA_TX_count;			//	сколько байт перенаправить



#endif /* UART_2_H_ */










