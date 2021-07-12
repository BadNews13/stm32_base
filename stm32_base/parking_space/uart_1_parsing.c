/*
 * uart_1_parsing.c
 *
 *  Created on: 8 июл. 2021 г.
 *      Author: bad_n
 */

#include <uart_1_parsing.h>
#include "parking_space.h"


uint8_t find_pack_from_uart_1 (void)
{
	CLEAR_BIT	(Parking_Space_STATUS,(1<<check_CMD));

//====================================================================================================

	for(uint8_t i = 0; i < uart1_rx_buf_size; i++) 		{pack_for_me_from_uart_1[i] = 0x00;}

	//	for cycle and start from zero index
	static uint8_t start_position = 0xFF;
	start_position++;
	if (start_position > uart1_rx_buf_size)	{start_position = 0;}

//====================================================================================================

	//	get index for byte with length
	uint8_t byte_LENGTH_index = BYTE_LEN + start_position;
	if (byte_LENGTH_index > uart1_rx_buf_size)		{byte_LENGTH_index = 	byte_LENGTH_index - 	uart1_rx_buf_size;}

	//	get index for byte with receiver address
	uint8_t byte_RECEIVER_index = BYTE_RECEIVER_ADR + start_position;
	if (byte_RECEIVER_index > uart1_rx_buf_size)	{byte_RECEIVER_index = 	byte_RECEIVER_index - 	uart1_rx_buf_size;}

	//	get index for byte with flags
	uint8_t byte_FLAGS_index = BYTE_FLAGS + start_position;
	if (byte_FLAGS_index > uart1_rx_buf_size)		{byte_FLAGS_index = 	byte_FLAGS_index - 		uart1_rx_buf_size;}

	//	get index for byte with crc
	uint8_t byte_CRC_index = start_position + uart1_rx_buf[start_position] - 1;
	if (byte_CRC_index > uart1_rx_buf_size)			{byte_CRC_index	= 		byte_CRC_index - 		uart1_rx_buf_size;}

//====================================================================================================

	//	check length
	if (uart1_rx_buf[byte_LENGTH_index] > MAX_PACK_LENGTH)					{return 0;}
	if (uart1_rx_buf[byte_LENGTH_index] < MIN_PACK_LENGTH)					{return 0;}

	//	check receiver address
	if (uart1_rx_buf[byte_RECEIVER_index] != adr_in_uart_1)					{return 0;}

	//	check "flag_pack" in flags
	if (!(READ_BIT(uart1_rx_buf[byte_FLAGS_index], (1<<CMD_FLAGS_PACK))))	{return 0;}

	//	check crc
	if (start_position + uart1_rx_buf[byte_LENGTH_index] > uart1_rx_buf_size)
	{
		uint8_t crc = 0;

		uint8_t first_part_cnt = 	uart1_rx_buf_size 				- start_position		;	//	количество байт до конца буфера (до перехода)
		uint8_t second_part_cnt = 	uart1_rx_buf[byte_LENGTH_index] - first_part_cnt	-1	;	//	количество байт вначале буфера (после перехода)

		crc = 	crc8_parts(	0,		&uart1_rx_buf[start_position],	first_part_cnt	);
		crc = 	crc8_parts(	crc,	&uart1_rx_buf[0],				second_part_cnt	);

		if(uart1_rx_buf[byte_CRC_index] != crc) {return 0;}
	}
	else
	{
		if(	uart1_rx_buf[byte_CRC_index] != crc8(&uart1_rx_buf[start_position],uart1_rx_buf[byte_LENGTH_index]-1))	{return 0;}
	}



//====================================================================================================

	uint8_t size = uart1_rx_buf[byte_LENGTH_index];
	for (uint8_t i = 0; i < size; i++)
	{
		if(i + start_position < uart1_rx_buf_size)
		{
			pack_for_me_from_uart_1[i] = 	uart1_rx_buf[i + start_position];
											uart1_rx_buf[i + start_position] 	= 0x00;
		}
		else
		{
			pack_for_me_from_uart_1[i] =	uart1_rx_buf[i - uart1_rx_buf_size + start_position];
											uart1_rx_buf[i - uart1_rx_buf_size + start_position]	= 0x00;
		}
	}

	SET_BIT	(Parking_Space_STATUS,(1<<CMD_ready));

	return 1;
}
