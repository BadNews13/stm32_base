/*
 * cmd_interp.h
 *
 *  Created on: 6 июл. 2021 г.
 *      Author: bad_n
 */

#ifndef CMD_INTERP_H_
#define CMD_INTERP_H_

#include <uart_2.h>

void pack_exe(void);



void pack_from_uart_1_exe (void);		//	обрабтать пакет из сети 1
void pack_from_uart_2_exe (void);		//	обрабтать пакет из сети 2

uint8_t check_hop(uint8_t from_NET);	//	проверяет нужно ли пересылать дальше пакет или обрабатываем
//void prepare_ACK (void);				//	готови ACK
void prepare_ACK (uint8_t *route, uint8_t cmd, uint8_t prm);

void system_reset(void);

//для пришедших пакетов (чтобы понять что с ним делать)
#define	our_ACK				0x00	//	ответный пакет для нас
#define	our_pack			0xFF	//	пакет с данными для нас
#define	hop_up				0xF0	//	пакет для пересылки назад
#define hop_down			0x0F	//	пакет для пересылки вперед
#define unreadable_pack		0xAA	//	нет флага "пакет" (не будем обрабатывать)
//


#endif /* CMD_INTERP_H_ */
