/*
 * uart_1_parsing.h
 *
 *  Created on: 8 июл. 2021 г.
 *      Author: bad_n
 */

#ifndef UART_1_PARSING_H_
#define UART_1_PARSING_H_

#include <uart_1.h>
#include <protocol.h>
#include <global_defines.h>
#include <crc8.h>


uint8_t find_pack_from_uart_1 (void);

uint8_t adr_in_uart_1;

uint8_t pack_for_me_from_uart_1[128];



#endif /* UART_1_PARSING_H_ */
