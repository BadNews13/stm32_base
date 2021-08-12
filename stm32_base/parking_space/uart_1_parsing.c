/*
 * uart_1_parsing.c
 *
 *  Created on: 8 июл. 2021 г.
 *      Author: bad_n
 */

#include <uart_1_parsing.h>
#include "parking_space.h"


uint8_t find_pack_from_uart_1_ (void)
{
	CLEAR_BIT	(Parking_Space_STATUS,(1<<check_CMD));

//====================================================================================================

	for(uint8_t i = 0; i < uart1_rx_buf_size; i++) 		{pack_for_me_from_uart_1[i] = 0x00;}	//	clear rx_pack

	static uint8_t start_position = 0xFF;	//	for cycle and start from zero index
	start_position++;	if (start_position >= uart1_rx_buf_size)	{start_position = 0;}

//	выбираем позиции предпологаемых системных байт пакета
//====================================================================================================

	//	get index for byte with length
	uint8_t byte_LENGTH_index = BYTE_LEN + start_position;
	if (byte_LENGTH_index >= uart1_rx_buf_size)		{byte_LENGTH_index = 	byte_LENGTH_index - 	uart1_rx_buf_size;}

	//	get index for byte with receiver address
	uint8_t byte_RECEIVER_index = BYTE_RECEIVER_ADR + start_position;
	if (byte_RECEIVER_index >= uart1_rx_buf_size)	{byte_RECEIVER_index = 	byte_RECEIVER_index - 	uart1_rx_buf_size;}

	//	get index for byte with flags
	uint8_t byte_FLAGS_index = BYTE_FLAGS + start_position;
	if (byte_FLAGS_index >= uart1_rx_buf_size)		{byte_FLAGS_index = 	byte_FLAGS_index - 		uart1_rx_buf_size;}

	//	get index for byte with crc
	uint8_t byte_CRC_index = start_position + uart1_rx_buf[start_position] - 1;
	if (byte_CRC_index >= uart1_rx_buf_size)		{byte_CRC_index	= 		byte_CRC_index - 		uart1_rx_buf_size;}

//====================================================================================================


	uint8_t lenght = 		uart1_rx_buf[byte_LENGTH_index];
	uint8_t receiver = 		uart1_rx_buf[byte_RECEIVER_index];
	uint8_t flags = 		uart1_rx_buf[byte_FLAGS_index];
	uint8_t crc =			uart1_rx_buf[byte_CRC_index];


// проверяем предпологаемы системные байты на соответствие
//====================================================================================================
	//	check length
	if (lenght > MAX_PACK_LENGTH)					{return 0;}
	if (lenght < MIN_PACK_LENGTH)					{return 0;}

	//	check receiver address
	if (receiver != adr_in_uart_1)					{return 0;}

	//	check "flag_pack" in flags
	if (!(READ_BIT(flags, (1<<CMD_FLAGS_PACK))))	{return 0;}

/*
uint8_t idx = 0;
// проверяем длину
if (uart1_rx_buf[byte_LENGTH_index] != 0x0B)					{return 0;}

// проверяем следующий адрес
idx = start_position + 2;	if(idx >= uart1_rx_buf_size)	{idx = (start_position + 2) - uart1_rx_buf_size;}
if (uart1_rx_buf[idx] != 0x00)									{return 0;}
// проверяем следующий адрес
idx = start_position + 3;	if(idx >= uart1_rx_buf_size)	{idx = (start_position + 3) - uart1_rx_buf_size;}
if (uart1_rx_buf[idx] != 0x00)									{return 0;}
// проверяем следующий адрес
idx = start_position + 4;	if(idx >= uart1_rx_buf_size)	{idx = (start_position + 4) - uart1_rx_buf_size;}
if (uart1_rx_buf[idx] != 0x00)									{return 0;}

// проверяем адрес отправителя
idx = start_position + 5;	if(idx >= uart1_rx_buf_size)	{idx = (start_position + 5) - uart1_rx_buf_size;}
if (uart1_rx_buf[idx] != 0x01)									{return 0;}


if (uart1_rx_buf[byte_FLAGS_index] != 0x01)						{return 0;}


idx = start_position + 8;	if(idx >= uart1_rx_buf_size)	{idx = (start_position + 8) - uart1_rx_buf_size;}
if 		(uart1_rx_buf[idx] == 0x21)								{}
else if (uart1_rx_buf[idx] == 0x24)								{}
else if (uart1_rx_buf[idx] == 0x20)								{}
else 															{return 0;}


idx = start_position + 9;	if(idx >= uart1_rx_buf_size)	{idx = (start_position + 9) - uart1_rx_buf_size;}
if 		(uart1_rx_buf[idx] == 0x01)								{}
else if (uart1_rx_buf[idx] == 0x02)								{}
else if (uart1_rx_buf[idx] == 0x00)								{}
else 															{return 0;}


if (uart1_rx_buf[byte_CRC_index] == 0x00)						{return 0;}

*/
	//	check crc
	if (start_position + lenght <= (uart1_rx_buf_size + 1))		//	+1, потому что если байт контрольной сумму перескочит, то всеравно считаем так же
	{
		// 19+11 <= 30 (т.е. если прибавить длину пакета, то индекс не будет больше 29)
		// 20 + 11


		if(	crc != crc8(&uart1_rx_buf[byte_LENGTH_index], lenght-1))
		{
/*			uint8_t incorrect_crc = crc8(&uart1_rx_buf[byte_LENGTH_index], lenght-1);
/*
			put_byte_UART2(0xA1);put_byte_UART2(0xA1);

			delay_ms(100);
			for (uint8_t i = 0; i < uart1_rx_buf_size; i ++)	{put_byte_UART2(uart1_rx_buf[i]);}
			delay_ms(100);

			put_byte_UART2(0xB1);put_byte_UART2(0xB1);

			put_byte_UART2(start_position);		put_byte_UART2(lenght);
			put_byte_UART2(byte_CRC_index);		put_byte_UART2(crc);	put_byte_UART2(incorrect_crc);
*/


			return 0;}
//		put_byte_UART2 (0x01);	put_byte_UART2 (0x02);	put_byte_UART2 (0x03);
	}
	else
	{
		// 21 + 11 = 32
		uint8_t crc = 0;

		uint8_t first_part_cnt = 	uart1_rx_buf_size 				- start_position	;	//	количество байт до конца буфера (до перехода)
		uint8_t second_part_cnt = 	uart1_rx_buf[byte_LENGTH_index] - first_part_cnt	;	//	количество байт вначале буфера (после перехода)

		crc = 	crc8_parts(	0,		&uart1_rx_buf[start_position],	first_part_cnt	);
		crc = 	crc8_parts(	crc,	&uart1_rx_buf[0],				second_part_cnt	-1);

		if(uart1_rx_buf[byte_CRC_index] != crc)
		{
/*			uint8_t crc_from_pack = uart1_rx_buf[byte_CRC_index];
			uint8_t crc = crc8(&uart1_rx_buf[byte_LENGTH_index],uart1_rx_buf[byte_LENGTH_index]-1);

			put_byte_UART2(0xA2);put_byte_UART2(0xA2);

			put_byte_UART2(start_position);	put_byte_UART2(uart1_rx_buf[start_position]);
			put_byte_UART2(byte_LENGTH_index);
			put_byte_UART2(byte_CRC_index);	put_byte_UART2(crc_from_pack);	put_byte_UART2(crc);



			delay_ms(100);
			for (uint8_t i = 0; i < uart1_rx_buf_size; i ++)	{put_byte_UART2(uart1_rx_buf[i]);}
			delay_ms(100);
*/
			return 0;}

//		put_byte_UART2 (0x03);	put_byte_UART2 (0x02);	put_byte_UART2 (0x01);
	}
//====================================================================================================



//====================================================================================================

	uint8_t size = uart1_rx_buf[byte_LENGTH_index];
	for (uint8_t i = 0; i < size; i++)
	{
		if(i + start_position < uart1_rx_buf_size)	// i = 9; start_position = 21; uart1_rx_buf_size = 30
		{
			pack_for_me_from_uart_1[i] = 	uart1_rx_buf[i + start_position];
											uart1_rx_buf[i + start_position] 	= 0x00;
		}
		else
		{
			pack_for_me_from_uart_1[i] =	uart1_rx_buf[(i + start_position) - uart1_rx_buf_size];
											uart1_rx_buf[(i + start_position) - uart1_rx_buf_size]	= 0x00;
		}
	}

	SET_BIT	(Parking_Space_STATUS,(1<<CMD_ready));

	return 1;
}






uint8_t find_pack_from_uart_1 (void)
{
	CLEAR_BIT	(Parking_Space_STATUS,(1<<check_CMD));

//====================================================================================================

	for(uint8_t i = 0; i < uart1_rx_buf_size; i++) 		{pack_for_me_from_uart_1[i] = 0x00;}	//	clear rx_pack
	static uint8_t start_position = 0xFF;	//	for cycle and start from zero index
	start_position++;	if (start_position >= uart1_rx_buf_size)	{start_position = 0;}

//	выбираем позиции предпологаемых системных байт пакета
//====================================================================================================

	//	get index for byte with length
	uint8_t byte_LENGTH_index = BYTE_LEN + start_position;
	if (byte_LENGTH_index >= uart1_rx_buf_size)		{byte_LENGTH_index = 	byte_LENGTH_index - 	uart1_rx_buf_size;}

	//	get index for byte with receiver address
	uint8_t byte_RECEIVER_index = BYTE_RECEIVER_ADR + start_position;
	if (byte_RECEIVER_index >= uart1_rx_buf_size)	{byte_RECEIVER_index = 	byte_RECEIVER_index - 	uart1_rx_buf_size;}

	//	get index for byte with flags
	uint8_t byte_FLAGS_index = BYTE_FLAGS + start_position;
	if (byte_FLAGS_index >= uart1_rx_buf_size)		{byte_FLAGS_index = 	byte_FLAGS_index - 		uart1_rx_buf_size;}


	//	get index for byte with crc
	uint8_t byte_CRC_index = start_position + uart1_rx_buf[byte_LENGTH_index] - 1;
	if (byte_CRC_index >= uart1_rx_buf_size)		{byte_CRC_index	= 		byte_CRC_index - 		uart1_rx_buf_size;}

//====================================================================================================

// проверяем предпологаемы системные байты на соответствие
//====================================================================================================

	if (uart1_rx_buf[byte_LENGTH_index] > uart1_rx_buf_size)				{return 0;}

	//	check length
	if (uart1_rx_buf[byte_LENGTH_index] > MAX_PACK_LENGTH)					{return 0;}
	if (uart1_rx_buf[byte_LENGTH_index] < MIN_PACK_LENGTH)					{return 0;}

	//	check receiver address
	if (uart1_rx_buf[byte_RECEIVER_index] != adr_in_uart_1)					{return 0;}

	//	check "flag_pack" in flags
	if (!(READ_BIT(uart1_rx_buf[byte_FLAGS_index], (1<<CMD_FLAGS_PACK))))	{return 0;}

	//	check crc
	if (start_position + uart1_rx_buf[byte_LENGTH_index] > uart1_rx_buf_size)	// было >=
	{
		uint8_t crc = 0;

		uint8_t first_part_cnt = 	uart1_rx_buf_size 				- start_position	;	//	количество байт до конца буфера (до перехода)
		uint8_t second_part_cnt = 	uart1_rx_buf[byte_LENGTH_index] - first_part_cnt	;	//	количество байт вначале буфера (после перехода)

		uint8_t tmp_array[uart1_rx_buf_size];
		for(uint8_t i = 0; i < first_part_cnt; i++ )		{tmp_array[i] = uart1_rx_buf[start_position + i];}
		for(uint8_t i = 0; i < second_part_cnt; i++ )		{tmp_array[first_part_cnt + i] = uart1_rx_buf[i];}

		if(	uart1_rx_buf[byte_CRC_index] != crc8(&tmp_array[0],tmp_array[0]-1))	{return 0;}



/*
		crc = 	crc8_parts(	0,		&uart1_rx_buf[start_position],	first_part_cnt		);
		crc = 	crc8_parts(	crc,	&uart1_rx_buf[0],				second_part_cnt	-1	);

		if(uart1_rx_buf[byte_CRC_index] != crc) {return 0;}
*/
	}
	else
	{
		if(	uart1_rx_buf[byte_CRC_index] != crc8(&uart1_rx_buf[start_position],uart1_rx_buf[byte_LENGTH_index]-1))	{return 0;}
	}
//====================================================================================================



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
			pack_for_me_from_uart_1[i] =	uart1_rx_buf[(i  + start_position) - uart1_rx_buf_size];
											uart1_rx_buf[(i  + start_position) - uart1_rx_buf_size]	= 0x00;
		}
	}

//	for (uint8_t i = 0; i < pack_for_me_from_uart_1[0]; i++)	{put_byte_UART2(pack_for_me_from_uart_1[i]); }	put_byte_UART2(0xFF);
	for (uint8_t i = 0; i < uart1_rx_buf_size; i++)				{uart1_rx_buf[i] = 0x00;}

	SET_BIT	(Parking_Space_STATUS,(1<<CMD_ready));

	return 1;
}




