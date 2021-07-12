/*
 * uart_2_parsing.h
 *
 *  Created on: 8 июл. 2021 г.
 *      Author: bad_n
 */

#ifndef UART_2_PARSING_H_
#define UART_2_PARSING_H_

#include <uart_2.h>
#include <protocol.h>
#include <global_defines.h>
#include <crc8.h>


void find_pack_from_uart_2 (void);
	uint8_t find_ACK (void);
	uint8_t find_NULL_PACK (void);

uint8_t adr_in_uart_2;

uint8_t pack_for_me_from_uart_2[128];







#endif /* UART_2_PARSING_H_ */
