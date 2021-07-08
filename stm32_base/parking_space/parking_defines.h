/*
 * parking_defines.h
 *
 *  Created on: 5 июл. 2021 г.
 *      Author: bad_n
 */

#ifndef PARKING_DEFINES_H_
#define PARKING_DEFINES_H_

#include <global_defines.h>
#include "protocol.h"



#define that_device_is_HEAD

#define MAX_DEVICES 10		//	количество устройств в данном узле


uint8_t devices_is_live[(MAX_DEVICES/8)+1];		//	массив работающих устройств (1 бит - 1 устройство) (номер бита в массиве = адресу датчика)
uint8_t sensors_status[(MAX_DEVICES/8)+1];		//	массив результатов датчиков (1 бит - 1 датчик) (номер бита в массиве = адресу датчика)


//адреса
volatile uint8_t RS232_address;			//	адрес в сети RS232 (сеть между компьютером и контроллерами системы)		(сейчас 15)
volatile uint8_t RS485_address;				//	адрес в сети RS485 (основная рабочая сеть системы)					(всегда 0x01)

#define EEPROM_ADR	0x01		//	адрес ячейки в энергонезависимой памяти где храним имя контроллера (подчиненное имя)





//метки сетей
uint8_t used_network;
#define MK_FREE		0x00
#define RS232		0x01
#define RS485		0x02
#define MIRF		0x03
#define OUTPUT		0x03

#define UART_1		0x01
#define UART_2		0x02

#endif /* PARKING_DEFINES_H_ */
