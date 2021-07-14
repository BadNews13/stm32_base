/*
 * cmd_interp.c
 *
 *  Created on: 6 июл. 2021 г.
 *      Author: bad_n
 */
#include <cmd_interp.h>

#include <uart_1.h>
#include <uart_1_parsing.h>

#include <uart_2.h>
#include <uart_2_parsing.h>


#include <parking_space.h>
#include "global_defines.h"

#include <lcd.h>

#include "protocol.h"


#include "flash.h"

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





void pack_from_uart_1_exe (void)
{
	CLEAR_BIT	(Parking_Space_STATUS,(1<<CMD_ready));

	uint8_t *pack = &pack_for_me_from_uart_1[0];

	uint8_t command =		pack[BYTE_COMMAND];
	uint8_t parameter =		pack[BYTE_PARAMETER];
	uint8_t *p_data =		&pack[BYTE_DATA_OFFSET];

	switch (check_hop(UART_1))			//	узнаем что надо делать с пакетом
	{
		case hop_down:
		{
			rebuild_for_resend(&pack[0]);		//	пересоберем пакет для отправки вниз
			pack[pack[BYTE_LEN]-1] =	crc8(&pack[0],pack[BYTE_LEN]-1);	//	11/12 byte:	посчитать и записать crc в пакет
			put_string_UART2(&pack[0], pack[BYTE_LEN]);
			SET_BIT(Parking_Space_STATUS, (1<<waiting_ACK));
			RTOS_SetTask(time_out_ACK,200,0);					//	запуск отсчета таймаута
		}
		break;

		case our_pack:
		{
			prepare_ACK();			//	подготовим ACK OUTPUT_pack с ошибкой в ответ на пакет из RS232
			switch (command)		//	начинается разбор команд
			{
				case CMD_CONTROL:	//	управление работой устройства
				{
					switch (parameter)
					{
						case PRM_START:
						{
							SET_BIT(Parking_Space_STATUS, (1<<Parking_Space_AUTO));

							//write_START
							LCD_Command(0x01);		//	очистка дисплея					(LCD_CLEAR)
							delay_ms(2);			//	долгая операция
							LCD_Command(LCD_SETDDRAMADDR | 0);	//	писать с нулевого адреса
							LCDsendString("Parking START");
						}
						break;

						case PRM_STOP:
						{

							CLEAR_BIT(Parking_Space_STATUS, (1<<Parking_Space_AUTO));
							prepare_ACK();	// т.к. массив один для всех исходящих пакетов, то после отправки сообщения на табло надо снова пересобрать ACK

							//write_STOP
							LCD_Command(0x01);		//	очистка дисплея					(LCD_CLEAR)
							delay_ms(2);			//	долгая операция
							LCD_Command(LCD_SETDDRAMADDR | 0);	//	писать с нулевого адреса
							LCDsendString("Parking STOP");
						}
						break;

						case PRM_RESET:	{system_reset();/*RS232_ignor = 0;*/}	break;	//	перенести сюда аппаратный сброс

						//default:		{send_ack_error(INCORRECT_PARAMETER);}	break;
					}
				}
				break;

				case CMD_SET_ADR:	//	установить новый адрес
				{
					switch (parameter)
					{
						case NO_PARAMETERS:
						{
							adr_in_uart_1 = p_data[0];							//	запишем новоый адрес в рабочую переменную

							uint32_t Page[WORD_CNT];

							//	считываем страницу
							for (uint8_t i = 0; i < WORD_CNT; i++)		{Page[i] = read_word(Page_30_ADDR + (i * WORD_ADDR_OFFSET));}	//	считываем по словам

							CLEAR_BIT	(Page[0], 0xFF<<(3*8));											//	стираем адрес нового устройства
							SET_BIT		(Page[0], p_data[0]<<(3*8));									//	записываем новый адрес данного устройства

							FLASH_Unlock();																//	разблокируем память
							FLASH_Erase_Page(Page_30_ADDR);												//	стираем память

							//	записываем всю страницу обратно в память
	//						for(uint8_t i = 0; i < WORD_CNT; i++)		{write_word((uint32_t)(Page_30_ADDR + (i*WORD_ADDR_OFFSET)), Page[i]);}

							FLASH_Lock();																//	блокируем память
						}
						break;

						//default:				send_ack_error(INCORRECT_PARAMETER);	break;
					}
				}
				break;

				case CMD_STATUS:		//	PC запросил состоянияе датчиком из ОЗУ контроллера?
				{
					uint8_t len_offset = (max_end_device/8)+1;				//	сдвиг длины на количество байт, в которых содержиться список устройств
					tx_pack[BYTE_LEN] = MIN_PACK_LENGTH + len_offset;		//	корректируем длину пакета

					switch (parameter)	//	узнаем какой параметр у этой каманды
					{
						case LIST_STATUSES:		//	запрос состояния парковочных мест из памяти
						{
							for(uint8_t i = 0; i<len_offset; i++)	{tx_pack[BYTE_DATA_OFFSET+i] = sensors_status[i];}
						}
						break;

						case LIST_DEVICES:		//	запрос работающих датчиков из памяти
						{
							for(uint8_t i = 0; i<len_offset; i++)	{tx_pack[BYTE_DATA_OFFSET+i] = devices_is_live[i];}
						}
						break;

						//default:				{send_ack_error(INCORRECT_PARAMETER);}	break;
					}
				}
				break;

				case CMD_PING:	//	пинг узла
				{
					switch(parameter)
					{
						case PRM_NULL:
						{
						//	CLEAR_BIT(tx_pack[BYTE_FLAGS],CMD_FLAGS_ACK_FLAG);
						//	if (RS232_address == NONAME_DEVICE)	{RTOS_SetTask(put_null_pack,2*3,0);}		//	если у нас нет адреса, то позже отправим нулевой пакет
						}
						break;

						case PRM_PING:
						{
							tx_pack[BYTE_LEN] = MIN_PACK_LENGTH + 1;	//	сдвиг длины на один байт, для добавления типа устройства
							tx_pack[BYTE_DATA_OFFSET] = NODE;			//	укажем тип своего устройства в байте данных
						}
						break;
					}
				}
				break;

				case CMD_CREATE_GROUP:
				{
					switch(parameter)
					{
						case PRM_SENSORS_TO_SUBGROUP:
						{
							uint16_t eeprom_adr = SUBGROUP_2_SENSORS_list + (p_data[0]*4);					// начало списка + (номер подгруппы * размер ячейки)
				//			for (uint16_t i = 0; i < 4; i++)	{EEPROM_write(eeprom_adr+i,p_data[i+1]);}
						}
						break;

						case PRM_SUBGROUP_TO_GROUP:
						{
							uint16_t eeprom_adr = SUBGROUPS_2_GROUP_list + (p_data[0]*1);					// начало списка + (номер группы * размер ячейки)
				//			EEPROM_write(eeprom_adr,p_data[1]);
						}
						break;

						case PRM_PANEL_TO_GROUP:
						{
							uint16_t eeprom_adr = PANELS_2_GROUP_list + (p_data[0]*1);					// начало списка + (номер панели * размер ячейки)
				//			EEPROM_write(eeprom_adr,p_data[1]);
						}
						break;
					}
				}
				break;


				case CMD_GROUP_STATUS:
				{
					switch(parameter)
					{
						case PRM_SENSORS_TO_SUBGROUP:
						{
							uint16_t eeprom_adr = SUBGROUP_2_SENSORS_list + (p_data[0]*4);					// начало списка + (номер подгруппы * размер ячейки)
				//			for (uint16_t i = 0; i < 4; i++)	{tx_pack[BYTE_DATA_OFFSET+i] =  EEPROM_read(eeprom_adr+i);}
							tx_pack[BYTE_LEN] = MIN_PACK_LENGTH + 4;	//	сдвиг длины на один байт, для добавления типа устройства
						}
						break;

						case PRM_SUBGROUP_TO_GROUP:
						{
							uint16_t eeprom_adr = SUBGROUPS_2_GROUP_list + (p_data[0]*1);					// начало списка + (номер группы * размер ячейки)
				//			tx_pack[BYTE_DATA_OFFSET] =  EEPROM_read(eeprom_adr);
							tx_pack[BYTE_LEN] = MIN_PACK_LENGTH + 1;	//	сдвиг длины на один байт, для добавления типа устройства
						}
						break;

						case PRM_PANEL_TO_GROUP:
						{
							uint16_t eeprom_adr = PANELS_2_GROUP_list + (p_data[0]*1);					// начало списка + (номер панели * размер ячейки)
				//			tx_pack[BYTE_DATA_OFFSET] =  EEPROM_read(eeprom_adr);
							tx_pack[BYTE_LEN] = MIN_PACK_LENGTH + 1;	//	сдвиг длины на один байт, для добавления типа устройства
						}
						break;

						case PRM_GROUP_LIST:
						{

							for (uint16_t eeprom_adr = PANELS_2_GROUP_list; eeprom_adr < (PANELS_2_GROUP_list + panels); eeprom_adr++)
							{
				//				uint8_t group = EEPROM_read(eeprom_adr);
/*
								if (group != 0xFF)	//	если есть связь панель-группа
								{
									tx_pack[BYTE_DATA_OFFSET+1] = group;
									tx_pack[BYTE_LEN] = tx_pack[BYTE_LEN] + 1;
								}
								*/
							}
						}
						break;
					}
				}
				break;




				//default:		{send_ack_error(INCORRECT_COMMAND);}	break;

			}


			if (READ_BIT(tx_pack[BYTE_FLAGS],(1<<CMD_FLAGS_ACK_FLAG)))
			{
				//	считаем crc пакета и кладем в пакет
				tx_pack[tx_pack[BYTE_LEN]-1] =	crc8(&tx_pack[0],tx_pack[BYTE_LEN]-1);		//	11/12 byte:	посчитать и записать crc в пакет
				for (uint8_t i = 0; i< tx_pack[BYTE_LEN]; i++)		{put_byte_UART1(tx_pack[i]);}						//	отправляем в RS485
			}

		}
		break;

		default:		{}		break;
	}

}




void pack_from_uart_2_exe (void)
{
//	Parking_Space_CONTROL = DO_PARSING_CMD;	//	по умолчанию перейдем в поиск команды от верхней сети
	CLEAR_BIT	(Parking_Space_STATUS,(1<<ACK_ready));

//	sbit(STATUS_MK,check_CMD);	cbit(STATUS_MK,ACK_ready);	cbit(STATUS_MK,waiting_ACK);
	RTOS_DeleteTask(time_out_ACK);					//	отменим вызов обработчика таймаута
//	RTOS_SetTask(reset_uart_rx,4000,0);				//	перезапустим отложенные сброс uart

	uint8_t *pack = &pack_for_me_from_uart_2[0];

	uint8_t adr_dev_from_pack =		pack[BYTE_SENDER_ADR];
	uint8_t count_from_pack =		pack[BYTE_COUNT_PACK];
	uint8_t command =				pack[BYTE_COMMAND];
	uint8_t parameter =				pack[BYTE_PARAMETER];
//	uint8_t flags_from_pack =		pack[BYTE_FLAGS];
	uint8_t data =					pack[BYTE_DATA_OFFSET];

	switch (check_hop(UART_2))		//	узнаем что надо делать с пакетом
	{
		case hop_up:
		{
			rebuild_for_resend(&pack[0]);					//	пересоберем пакет для отправки вниз
			pack[pack[BYTE_LEN]-1] =	crc8(&pack[0],pack[BYTE_LEN]-1);				//	11/12 byte:	посчитать и записать crc в пакет
			for (uint8_t i = 0; i < pack[BYTE_LEN]; i++)	{put_byte_UART1(pack[i]);}	//	отправим пакет вверх
		}
		break;

		case our_ACK:
		{
			set_device_as_live(CURRENT_DEVICE);

			if(	(CURRENT_COUNT_PACK	== count_from_pack) &&		//	если это ответ на текущий, отправленный пакет
				(CURRENT_DEVICE	== adr_dev_from_pack)		)	//	адрес датчика совпадет с тем дачтиком, с которым мы работатем (с тем кому отправили прошлый пакет)
			{
				switch (command)	//	в зависимости от команды на которую этот пакет является ответом (команда лежит в пришедшем пакете)
				{
					case CMD_STATUS:	//	контроллер просил состояние парковочного места?
					{
						switch (parameter)
						{
							case PRM_STATUS:	// ответ наданную команду обрабатывается по разному взависимости от того кто мы и кто нам на нее ответил
							{
								switch (I_am)							//	в зависимости от того кто - мы будем обрабатывать пакет
								{
									case HEAD:							//	если мы ГОЛОВА
									{
										switch (CURRENT_DEVICE_TYPE)	//	в зависимости от текущего устройства
										{
											case HEAD:		{}		break;	//	невозможное событие (другой головы не может быть)
											case NODE:		{/**/}	break;	//	обрабатываем список статусов
											case SENSOR:
											{
												switch (data)
												{
													case FREE:		{set_status_as_free(CURRENT_DEVICE);}	break;
													case TAKEN:		{set_status_as_taken(CURRENT_DEVICE);}	break;
													case unknown:	{}	break;
												}
											}
											break;	//	обрабатываем единичное состояние одного датчика
										}
									}
									break;


									case NODE:							//	если мы УЗЕЛ
									{
										switch (CURRENT_DEVICE_TYPE)	//	в зависимости от текущего устройства, которое нам ответило
										{
											case HEAD:		{}		break;	//	невозможное событие (мы не можем командовать головой)
											case NODE:		{}		break;	//	невозможное событие (мы не можем командовать другим узлом)
											case SENSOR:					//	обрабатываем единичное состояние одного датчика
											{
												switch (data)
												{
													case FREE:		{set_status_as_free(adr_dev_from_pack);}		break;		//	пометить место как свободное
													case TAKEN:		{set_status_as_taken(adr_dev_from_pack);}		break;		//	пометить место как занятое
													case unknown:	{}												break;		//	состояние парковочного места неизвестно (возможно это экран)
												}
											}
											break;
										}
									}
									break;

								}
							}
							break;
						}
					}
					break;



					case CMD_PING:
					{
						switch (parameter)
						{
							case PRM_NULL:	{COUNT_NULL_PACK++;}	break;
							case PRM_PING:	{CURRENT_DEVICE_TYPE = pack[BYTE_DATA_OFFSET]; PARKING_STAGE = DEFINED;}	break;	//	сбросим бит ожидания типа
							case PRM_BLINK: {}	break;	//	мы такое не генерируем
						}
					}
					break;

					case CMD_CONTROL:
					{
						switch (parameter)
						{
							case PRM_RESET:	{}	break;
						}
					}
					break;

					case CMD_WRITE_TEXT:	{break;}	//	контроллер выводил сообщение на led_panel?
					//case CMD_RESET:			{cbit(DIAL_STAGE,hold_current_device);	break;}	//	контроллер перезагружал устройство?

					case CMD_SET_ADR:		{PARKING_STAGE = DEFINED;} break;
				}
			}
		}
		break;

		default:		{/*ERROR*/}							break;
	}
}




uint8_t check_hop(uint8_t from_NET)					//	узнаем маршрут пакета и возвращаем метку для следующей функции обработки
{
//	put_byte_UART2(0xA1);put_byte_UART2(0xA1);put_byte_UART2(0xA1);
	//	from_NET - откуда взялся пакет (где сформирован)
	switch (from_NET)
	{
		case UART_1:
		{
//			put_byte_UART2(0xA2);
			uint8_t *pack = &pack_for_me_from_uart_1[0];
			if( READ_BIT(pack[BYTE_FLAGS],(1<<CMD_FLAGS_PACK))	)			//	это вообще пакет?
			{
				if( READ_BIT( pack[BYTE_FLAGS],(1<<CMD_FLAGS_ACK_FLAG) ) )		//	пакет ACK?
				{
					if(!pack[BYTE_NEXT_RECEIVER_ADR])	{/*put_byte_UART2(unreadable_pack);*/ return unreadable_pack;}	//	назад некуда отправлять?	//	ACK для нас	(невозможное событие)	//	put_byte удалить после отладки
					if(pack[BYTE_NEXT_RECEIVER_ADR])	{/*put_byte_UART2(unreadable_pack);*/ return unreadable_pack;}	//	есть следующий адрес?		//	кинуть наверх (невозможное событие)	//	put_byte удалить после отладки
				}
				else																//	пакет не ACK?
				{
					if(!pack[BYTE_NEXT_RECEIVER_ADR])	{/*put_byte_UART2(our_pack);*/ return our_pack;}			//	вперед некуда отправлять?	//	пакет для нас	//	put_byte удалить после отладки
					if(pack[BYTE_NEXT_RECEIVER_ADR])	{/*put_byte_UART2(hop_down);*/ return hop_down;}			//	есть следующий адрес?		//	кинуть вперед	//	put_byte удалить после отладки
				}
			}
		}
		break;


		case UART_2:
		{
			uint8_t *pack = &pack_for_me_from_uart_2[0];
			if(	READ_BIT(pack[BYTE_FLAGS],(1<<CMD_FLAGS_PACK))	)				//	это вообще пакет?
			{
				if(READ_BIT(pack[BYTE_FLAGS],(1<<CMD_FLAGS_ACK_FLAG)))		//	пакет ACK?
				{
					if(!pack[BYTE_NEXT_RECEIVER_ADR])	{return our_ACK;}			//	назад некуда отправлять?	//	ACK для нас	(невозможное событие)	//	put_byte удалить после отладки
					if(pack[BYTE_NEXT_RECEIVER_ADR])	{return hop_up;}			//	есть следующий адрес?		//	кинуть наверх (невозможное событие)	//	put_byte удалить после отладки
				}
				else																//	пакет не ACK?
				{
					if(!pack[BYTE_NEXT_RECEIVER_ADR])	{return unreadable_pack;}	//	вперед некуда отправлять?	//	пакет для нас невозможное событие)	//	put_byte удалить после отладки
					if(pack[BYTE_NEXT_RECEIVER_ADR])	{return unreadable_pack;}	//	есть следующий адрес?		//	кинуть вперед( невозможное событие) //	put_byte удалить после отладки
				}
			}
		}
		break;
	}
	return unreadable_pack;			// возвращает символ - "нечитаемый пакет"
}





void prepare_ACK (void)
{
	//	строим маршрут до мастера и кладем ответные данные
	tx_pack[BYTE_LEN] =						MIN_PACK_LENGTH;				//	длина пакета (переделать на авто расчет)
	tx_pack[BYTE_RECEIVER_ADR] =			MASTER;							//	указываем адрес
	tx_pack[BYTE_NEXT_RECEIVER_ADR] =		ROUTE_END;						//	маршрут дальше первой передачи не пойдет
	tx_pack[BYTE_ADDR_3] =					ROUTE_END;						//	этот байт можно не заполнять. его не прочтут
	tx_pack[BYTE_PREVIOUS_SENDER_ADR] =		ROUTE_END;						//	этот байт нужно выставить в 0
	tx_pack[BYTE_SENDER_ADR] =				adr_in_uart_1;					//	собственный адрес в сети uart_1
	tx_pack[BYTE_COUNT_PACK] =				pack_for_me_from_uart_1[BYTE_COUNT_PACK];	//	номер пакета (по версии PC)

	tx_pack[BYTE_FLAGS] =						0;								//	обнулим байт флагов
	SET_BIT(tx_pack[BYTE_FLAGS],(1<<CMD_FLAGS_PACK));									//	ставим флаг "пакет"
	SET_BIT(tx_pack[BYTE_FLAGS],(1<<CMD_FLAGS_ACK_FLAG));								//	ставим флаг "ACK"		(далее по этому флагу проверяем надо ли отправлять пакет или нет)

	tx_pack[BYTE_COMMAND] =					pack_for_me_from_uart_1[BYTE_COMMAND];		//	записываем команду на которую отвечаем	(по команде вроде ПК определяет пакет) (надо дописать в обработчике ПК)
	tx_pack[BYTE_PARAMETER] =				pack_for_me_from_uart_1[BYTE_PARAMETER];		//	записываем параметр команды - Ошибка (на случай если не сможем обработать)

//	 tx_pack[10]	=	crc;
}


// На всякий случай, если понадобится, то перезагрузка делается вот так:
void system_reset(void)
{
   SCB->AIRCR = (0x5FA << SCB_AIRCR_VECTKEYSTAT_Pos) | SCB_AIRCR_SYSRESETREQ_Msk;
   __DSB();
  while(1);
}
