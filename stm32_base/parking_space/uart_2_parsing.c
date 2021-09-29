/*
 * uart_2_parsing.c
 *
 *  Created on: 8 июл. 2021 г.
 *      Author: bad_n
 */
#include <uart_2_parsing.h>
#include "parking_space.h"



uint8_t find_ACK (void)
{

	for(uint8_t i = 0; i < uart2_rx_buf_size; i++) 		{pack_for_me_from_uart_2[i] = 0x00;}	//	clear rx_pack


	static uint8_t start_position = uart2_rx_buf_size;							//	offset
	start_position++;	if (start_position >= uart2_rx_buf_size)	{start_position = 0;}	//	for cycle and start from zero index


//====================================================================================================

	//	get index for byte with length
	uint8_t byte_LENGTH_index = BYTE_LEN + start_position;
	if (byte_LENGTH_index >= uart2_rx_buf_size)		{byte_LENGTH_index = 	byte_LENGTH_index - 	uart2_rx_buf_size;}	//	строка бесполезна

	//	get index for byte with receiver address
	uint8_t byte_RECEIVER_index = BYTE_RECEIVER_ADR + start_position;
	if (byte_RECEIVER_index >= uart2_rx_buf_size)	{byte_RECEIVER_index = 	byte_RECEIVER_index - 	uart2_rx_buf_size;}

	//	get index for byte with flags
	uint8_t byte_FLAGS_index = BYTE_FLAGS + start_position;
	if (byte_FLAGS_index >= uart2_rx_buf_size)		{byte_FLAGS_index = 	byte_FLAGS_index - 		uart2_rx_buf_size;}

	//	get index for byte with cmd
	uint8_t byte_COMMAND_index = BYTE_COMMAND + start_position;
	if (byte_COMMAND_index >= uart2_rx_buf_size)	{byte_COMMAND_index = 	byte_COMMAND_index - 	uart2_rx_buf_size;}

	//	get index for byte with crc
	uint8_t byte_CRC_index = start_position + uart2_rx_buf[byte_LENGTH_index] - 1;
	if (byte_CRC_index >= uart2_rx_buf_size)		{byte_CRC_index	= 		byte_CRC_index - 		uart2_rx_buf_size;}

	//	get index for byte with separator char
	uint8_t byte_SEPARATOR_index = start_position + uart2_rx_buf[byte_LENGTH_index];
	if (byte_SEPARATOR_index >= uart2_rx_buf_size)		{byte_SEPARATOR_index	= 		byte_SEPARATOR_index - 		uart2_rx_buf_size;}

//====================================================================================================

	if (uart2_rx_buf[byte_LENGTH_index] > uart2_rx_buf_size)				{return 0;}	//	т.к. для отладки массив делаем на малое количество байт

	//	check length
	if (uart2_rx_buf[byte_LENGTH_index] > MAX_PACK_LENGTH)					{return 0;}
	if (uart2_rx_buf[byte_LENGTH_index] < MIN_PACK_LENGTH)					{return 0;}

	//	check receiver address
	if (uart2_rx_buf[byte_RECEIVER_index] != adr_in_uart_2)					{return 0;}

	//	check "flag_pack" in flags
	if (!(READ_BIT(uart2_rx_buf[byte_FLAGS_index], (1<<CMD_FLAGS_PACK))))	{return 0;}

	//	check "cmd_field" in cmd
	if (uart2_rx_buf[byte_COMMAND_index] < 0x20)							{return 0;}
	if (uart2_rx_buf[byte_COMMAND_index] > 0x2F)							{return 0;}

	//	check crc
	if (start_position + uart2_rx_buf[byte_LENGTH_index] <= (uart2_rx_buf_size + 1)	)
	{
		uint8_t crc = crc8(&uart2_rx_buf[start_position], uart2_rx_buf[byte_LENGTH_index]-1);	if (crc == 0) {crc = 1;}
		if(	uart2_rx_buf[byte_CRC_index] != crc )	{return 0;}
	}
	else
	{
		uint8_t first_part_cnt = 	uart2_rx_buf_size 				- start_position;	//	количество байт до конца буфера (до перехода)
		uint8_t second_part_cnt = 	uart2_rx_buf[byte_LENGTH_index] - first_part_cnt;	//	количество байт вначале буфера (после перехода)

//===== кастыль, чтобы посчитать контрольную сумму пакета прошедшего через конец/начало буфера ==================
		uint8_t crc = 	crc8_parts(0, 		&uart2_rx_buf[start_position], 		first_part_cnt);
				crc = 	crc8_parts(crc, 	&uart2_rx_buf[0], 					second_part_cnt - 1);
		if (crc == 0) {crc = 1;}
//===============================================================================================================

		if ( uart2_rx_buf[byte_CRC_index] != crc )	{return 0;}
	}

//====================================================================================================

		if ( uart2_rx_buf[byte_SEPARATOR_index] != SEPARATOR )	{return 0;}	//	кастыль на случай, если CRC совпадет

//====================================================================================================

	uint8_t size = uart2_rx_buf[byte_LENGTH_index];
	for (uint8_t i = 0; i < (size + 1); i++)
	{
		if((i + start_position) < uart2_rx_buf_size)
		{
			pack_for_me_from_uart_2[i] = 	uart2_rx_buf[i + start_position];
											uart2_rx_buf[i + start_position] 	= 0x00;
		}
		else
		{
			pack_for_me_from_uart_2[i] =	uart2_rx_buf[(i + start_position) - uart2_rx_buf_size];
											uart2_rx_buf[(i + start_position) - uart2_rx_buf_size]	= 0x00;
		}
	}
//====================================================================================================


	CLEAR_BIT	(Parking_Space_STATUS,(1<<waiting_ACK));	//	снимаем флаг - ждем ACK
	SET_BIT		(Parking_Space_STATUS,(1<<ACK_ready));
	return 1;

}


uint8_t find_NULL_PACK (void)
{
	//	for cycle and start from zero index
	static uint8_t start_position = 0xFF;
	start_position++;
	if (start_position > uart2_rx_buf_size)	{start_position = 0;}

//====================================================================================================

	//	get index for byte with length
	uint8_t byte_LENGTH_index = 0 + start_position;
	if (byte_LENGTH_index > uart2_rx_buf_size)		{byte_LENGTH_index = 	byte_LENGTH_index - 	uart2_rx_buf_size;}

	//	get index for byte with CNT
	uint8_t byte_CNT_index = 1 + start_position;
	if (byte_CNT_index > uart2_rx_buf_size)			{byte_CNT_index = 		byte_CNT_index - 		uart2_rx_buf_size;}

	//	get index for byte with CRC
	uint8_t byte_CRC_index = 2 + start_position;
	if (byte_CRC_index > uart2_rx_buf_size)			{byte_CRC_index = 		byte_CRC_index - 		uart2_rx_buf_size;}

//====================================================================================================

	//	check length
	if (uart2_rx_buf[byte_LENGTH_index] != 3)					{return 0;}

	//	check crc
	if (start_position + 3 > uart2_rx_buf_size)				//	если был переход через конец буфера к началу
	{
		uint8_t crc = 0;

		uint8_t first_part_cnt = 	uart2_rx_buf_size 					- start_position		;	//	количество байт до конца буфера (до перехода)
		uint8_t second_part_cnt = 	uart2_rx_buf[byte_LENGTH_index] 	- first_part_cnt	-1	;	//	количество байт вначале буфера (после перехода)

		crc = 	crc8_parts(	0,		&uart2_rx_buf[start_position],	first_part_cnt	);
		crc = 	crc8_parts(	crc,	&uart2_rx_buf[0],				second_part_cnt	);

		if(uart2_rx_buf[byte_CRC_index] != crc) 	{return 0;}
	}
	else
	{
		if(	uart2_rx_buf[byte_CRC_index] != crc8(&uart2_rx_buf[start_position],uart2_rx_buf[byte_LENGTH_index]-1))		{return 0;}
	}


//	if (uart2_rx_buf)


//====================================================================================================

	uint8_t size = uart2_rx_buf[byte_LENGTH_index];
	for (uint8_t i = 0; i < size + 1; i++)
	{
		if(i + start_position < uart2_rx_buf_size)
		{
			pack_for_me_from_uart_2[i] = 	uart2_rx_buf[i + start_position];
											uart2_rx_buf[i + start_position] 	= 0x00;
		}
		else
		{
			pack_for_me_from_uart_2[i] =	uart2_rx_buf[i - uart2_rx_buf_size + start_position];
											uart2_rx_buf[i - uart2_rx_buf_size + start_position]	= 0x00;
		}
	}

//====================================================================================================



	return 1;

}




void find_pack_from_uart_2 (void)
{
	if (find_ACK())
	{
		CLEAR_BIT	(Parking_Space_STATUS,(1<<waiting_ACK));	//	снимаем флаг - ждем ACK
		SET_BIT		(Parking_Space_STATUS,(1<<ACK_ready));		//
	}

	//====================== Если мы отправили запрос к безымянному устройству ======================
	if(ADDR_OF_SELECTED_DEVICE == IS_NONAME)			//	если мы работаем с новым устройством
	{
		if (find_NULL_PACK())	{COUNT_NULL_PACK++;}	//	если нашли нулевой (трехбайтный) пакет от подчиненных, то увеличим счетчик нулевых пакетов
	}
	//==================================================================
}




/*
uint8_t find_ACK (void)
{

	for(uint8_t i = 0; i < uart2_rx_buf_size; i++) 		{pack_for_me_from_uart_2[i] = 0x00;}	//	clear rx_pack

	//	for cycle and start from zero index
	static uint8_t start_idx = uart2_rx_buf_size;							//	offset
	start_idx++;	if (start_idx >= uart2_rx_buf_size)	{start_idx = 0;}


//====================================================================================================

	//	get index for byte with length
	uint8_t byte_LENGTH_index = BYTE_LEN + start_idx;
	if (byte_LENGTH_index >= uart2_rx_buf_size)		{byte_LENGTH_index = 	byte_LENGTH_index - 	uart2_rx_buf_size;}	//	строка бесполезна

	//	get index for byte with receiver address
	uint8_t byte_RECEIVER_index = BYTE_RECEIVER_ADR + start_idx;
	if (byte_RECEIVER_index >= uart2_rx_buf_size)	{byte_RECEIVER_index = 	byte_RECEIVER_index - 	uart2_rx_buf_size;}

	//	get index for byte with flags
	uint8_t byte_FLAGS_index = BYTE_FLAGS + start_idx;
	if (byte_FLAGS_index >= uart2_rx_buf_size)		{byte_FLAGS_index = 	byte_FLAGS_index - 		uart2_rx_buf_size;}

	//	get index for byte with cmd
	uint8_t byte_COMMAND_index = BYTE_COMMAND + start_idx;
	if (byte_COMMAND_index >= uart2_rx_buf_size)	{byte_COMMAND_index = 	byte_COMMAND_index - 	uart2_rx_buf_size;}

	//	get index for byte with crc
	uint8_t byte_CRC_index = start_idx + uart2_rx_buf[byte_LENGTH_index] - 1;
	if (byte_CRC_index >= uart2_rx_buf_size)		{byte_CRC_index	= 		byte_CRC_index - 		uart2_rx_buf_size;}

//====================================================================================================

	if (uart2_rx_buf[byte_LENGTH_index] > uart2_rx_buf_size)				{return 0;}	//	т.к. для отладки массив делаем на малое количество байт

	//	check length
	if (uart2_rx_buf[byte_LENGTH_index] > MAX_PACK_LENGTH)					{return 0;}
	if (uart2_rx_buf[byte_LENGTH_index] < MIN_PACK_LENGTH)					{return 0;}

	//	check receiver address
	if (uart2_rx_buf[byte_RECEIVER_index] != adr_in_uart_2)					{return 0;}

	//	check "flag_pack" in flags
	if (!(READ_BIT(uart2_rx_buf[byte_FLAGS_index], (1<<CMD_FLAGS_PACK))))	{return 0;}

	//	check "cmd_field" in cmd
	if (uart2_rx_buf[byte_COMMAND_index] < 0x20)							{return 0;}
	if (uart2_rx_buf[byte_COMMAND_index] > 0x2F)							{return 0;}


	//	check crc
	if (start_idx + uart2_rx_buf[byte_LENGTH_index] > uart2_rx_buf_size)	//	было >=
	{
		uint8_t static crc = 0;

		uint8_t first_part_cnt = 	uart2_rx_buf_size 				- start_idx	;	//	количество байт до конца буфера (до перехода)
		uint8_t second_part_cnt = 	uart2_rx_buf[byte_LENGTH_index] - first_part_cnt	;	//	количество байт вначале буфера (после перехода)

		crc = 	crc8_parts(	0,		&uart2_rx_buf[start_idx],		first_part_cnt);
		crc = 	crc8_parts(	crc,	&uart2_rx_buf[0],				second_part_cnt	- 1);

		if(uart2_rx_buf[byte_CRC_index] != crc) {return 0;}
	}
	else
	{
		if(	uart2_rx_buf[byte_CRC_index] != crc8(&uart2_rx_buf[start_idx],uart2_rx_buf[byte_LENGTH_index]-1))	{return 0;}
	}


//====================================================================================================

	uint8_t size = uart2_rx_buf[byte_LENGTH_index];
	for (uint8_t i = 0; i < size; i++)
	{
		if(i + start_idx < uart2_rx_buf_size)
		{
			pack_for_me_from_uart_2[i] = 	uart2_rx_buf[i + start_idx];
											uart2_rx_buf[i + start_idx] 	= 0x00;
		}
		else
		{
			pack_for_me_from_uart_2[i] =	uart2_rx_buf[i - uart2_rx_buf_size + start_idx];
											uart2_rx_buf[i - uart2_rx_buf_size + start_idx]	= 0x00;
		}
	}

	CLEAR_BIT	(Parking_Space_STATUS,(1<<waiting_ACK));	//	снимаем флаг - ждем ACK
	SET_BIT		(Parking_Space_STATUS,(1<<ACK_ready));
	return 1;

}


*/
