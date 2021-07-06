/*
 * cmd_interp.c
 *
 *  Created on: 6 июл. 2021 г.
 *      Author: bad_n
 */

#include <uart_1.h>
#include <uart_2.h>
#include <cmd_interp.h>


extern char 	uart2_rx_buf[uart2_tx_buf_size];



void pack_exe(void)
{

	static uint8_t last_value = 0;
	if(uart2_tx_buf[0] != last_value)
	{
		for (uint8_t i = 0; i<uart2_tx_buf_size; i++)
		{
			last_value = uart2_tx_buf[0];
			put_byte_UART2(uart2_rx_buf[i]);
			put_byte_UART1(uart2_rx_buf[i]);
		}
	}
}
