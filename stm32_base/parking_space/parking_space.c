/*
 * polling.c
 *
 * Created: 25.09.2020 12:19:01
 *  Author: bad_n
 */ 

#include "parking_space.h"

#include "global_defines.h"
#include "protocol.h"
//#include "cmd_interp.h"
#include <rtos.h>
#include <uart_1.h>
#include <uart_1_parsing.h>
#include <uart_2.h>
#include <uart_2_parsing.h>
//#include <uart_3.h>
#include "crc8.h"
#include <delay_ms.h>
#include <flash.h>


#include "lcd.h"


uint16_t TIME_FOR_TIME_OUT_ACK = 100;	//	время ожидания ACK должно зависеть от команды


void creat_pack (void)
{
	//	строим маршрут до мастера и кладем ответные данные
	tx_pack[BYTE_LEN] =							MIN_PACK_LENGTH;		//	длина пакета (переделать на авто расчет)
	tx_pack[BYTE_RECEIVER_ADR] =				CURRENT_DEVICE;			//	указываем адрес
	tx_pack[BYTE_NEXT_RECEIVER_ADR] =			ROUTE_END;				//	маршрут дальше первой передачи не пойдет
	tx_pack[BYTE_ADDR_3] =						ROUTE_END;				//	этот байт можно не заполнять. его не прочтут
	tx_pack[BYTE_PREVIOUS_SENDER_ADR] =			ROUTE_END;				//	этот байт нужно выставить в 0
	tx_pack[BYTE_SENDER_ADR] =					MASTER;					//	собственный адрес в сети RS232
//	tx_pack[BYTE_COUNT_PACK] =					++CURRENT_COUNT_PACK;	//	номер пакета (по версии PC)

	tx_pack[BYTE_COMMAND] = unknown;
	tx_pack[BYTE_PARAMETER] = unknown;
	
	tx_pack[BYTE_FLAGS] =						0;						//	обнулим байт флагов
	SET_BIT(tx_pack[BYTE_FLAGS],(1<<CMD_FLAGS_PACK));					//	ставим флаг "пакет"
}

void switch_dev_adr (void)
{
	CURRENT_DEVICE++;																		//	��������� � ���������� ������
	CURRENT_DEVICE_TYPE = unknown;															//	���������� ��� ����������

	if ((CURRENT_DEVICE == 0xFF) || (CURRENT_DEVICE < 0x02))	{CURRENT_DEVICE = 0x02;}	//	������� �� ������������� ���������
	if (CURRENT_DEVICE > MAX_DEVICES)							{CURRENT_DEVICE = 0xFF;}	//	����� ���������� ������� ����� ��������� ����� ����������
}




void give_addess (uint8_t dev_type)
{
	tx_pack[BYTE_COMMAND] = CMD_SET_ADR;			//	���������� ������� �������� ������
	tx_pack[BYTE_PARAMETER] = PRM_NULL;				//	���������� �������� �������

	switch(dev_type)
	{
		case NODE:		{tx_pack[BYTE_DATA_OFFSET] = 0;}		break;
		case SENSOR:	{tx_pack[BYTE_DATA_OFFSET] = 3;}		break;
		case SCREEN:	{tx_pack[BYTE_DATA_OFFSET] = 31;}		break;
	}
}


//========================================================================


void rebuild_for_resend (uint8_t *pack)
{
	// пересылка по адресам осуществляется в прямой последовательности по указанным адресам вне зависимости от направления пересылки
	for (uint8_t i = BYTE_RECEIVER_ADR; i<BYTE_SENDER_ADR; i++)		{pack[i] = pack[i+1];}						//	сдвинем адреса

	//		если получает мастер				или		стоит флаг ACK пакета									//	то шлем вверх
	if (	pack[BYTE_RECEIVER_ADR] == MASTER	||		READ_BIT(pack[BYTE_FLAGS],(1<<CMD_FLAGS_ACK_FLAG))		)
	{pack[BYTE_SENDER_ADR] = adr_in_uart_1;}																	//	вставляем свой адрес в сети мастера, чтобы отправить вверх
	
	//		если отправитель мастер		 		или		не стоит флаг ACK пакета								//	то шлем вниз
	if (	pack[BYTE_SENDER_ADR] == MASTER		||		!READ_BIT(pack[BYTE_FLAGS],(1<<CMD_FLAGS_ACK_FLAG))	)
	{
		pack[BYTE_SENDER_ADR] = MASTER;																			//	вставляем адрес мастера, чтобы отправить вниз
		CURRENT_COUNT_PACK = pack[BYTE_COUNT_PACK];
		CURRENT_DEVICE = pack[BYTE_RECEIVER_ADR];
	}
}





void time_out_ACK (void)
{	
//	if (CURRENT_DEVICE == 2)	{put_byte_UART1(CURRENT_DEVICE); for (uint8_t i = 0; i < 30; i++)	{put_byte_UART1(uart2_rx_buf[i]);}}
//	if (CURRENT_DEVICE == 3)	{put_byte_UART1(CURRENT_DEVICE); for (uint8_t i = 0; i < 30; i++)	{put_byte_UART1(uart2_rx_buf[i]);}}



	for (uint8_t i = 0; i<30; i++)

	CLEAR_BIT	(Parking_Space_STATUS,(1<<waiting_ACK));	//	снимаем флаг - ждем ACK

	if (!(CURRENT_DEVICE == NONAME_DEVICE))	{PARKING_STAGE = RESTART;}
	if (!tx_pack[BYTE_PREVIOUS_SENDER_ADR])	{set_device_as_dead(CURRENT_DEVICE);}	//	если отправленный пакет не имеет другого отправителя в своем адресе, то он наш => пометим устройство как мертвое
}



uint8_t free_address (uint8_t type_dev)
{
	uint8_t new_adr = 0xFF;
	
	switch (type_dev)
	{
		case SCREEN:
		{
			for (uint8_t i = 1; i<max_end_device; i++)	{	if(!sensor_is_live(i))	{new_adr = i;}	}	//	��������� �� ��������� �������
		}
		break;
		
		case SENSOR:
		{
			for (uint8_t i = max_end_device; i>1; i--)	{	if(!sensor_is_live(i))	{new_adr = i;}	}	//	������ �� ��������� �������
		}
		break;
		
		case NODE:
		{
			for (uint8_t i = max_end_device; i>1; i--)	{	if(!sensor_is_live(i))	{new_adr = i;}	}	//	������ �� ��������� �������
		}
		break;
	}
	
	set_device_as_live(new_adr);
	return new_adr;
} 


void prepare_message_to_screen (void)
{
	tx_pack[BYTE_COMMAND] = 	CMD_WRITE_TEXT;		//	записываем команду - задать новый адрес
	tx_pack[BYTE_PARAMETER] = 	PRM_RED;			//	записываем параметр команды	- без параметра
	TIME_FOR_TIME_OUT_ACK = delay_for_cmd;
		
	uint8_t	count_live_devices = 0;			uint8_t	count_free_places = 0;
	char	res_live[1];					char	res_free[1];
	
	for (uint8_t i = 2; i<MAX_DEVICES; i++)
	{
		if (i != CURRENT_DEVICE)							//	проходим по все устройствам из памяти кроме текущего, т.к. текущий - экран
		{
			if (sensor_is_live(i))
			{
				count_live_devices++;						//	сколько всего устройств живы в данный момент
				count_free_places += sensor_is_free(i);		//	сколько из них свободны
			}
		}
	}

	utoa(count_live_devices, res_live, 10);
	utoa(count_free_places, res_free, 10);		//	берет значение из переменной count_free_places, приводит его в десятичный вид и записывает в строку res_free

	char res_all[3];

	res_all[0] = res_free[0];	// res_free[0];	//	кол-во свободных мест с живых датчиков
	res_all[1] = '/';
	res_all[2] = res_live[0];	//	кол-во живый датчиков
	
		
	//uint8_t data_len = 	strlen(res_all);
	for (uint8_t i = 0; i<3; i++)	{tx_pack[BYTE_DATA_OFFSET + i] = res_all[i];}
	tx_pack[BYTE_LEN] = tx_pack[BYTE_LEN] + 3;		//	data_len;
	delay_ms(1);
}




/////////////////////////////////////////////////////////////////////////////////////



void choice_next_dev (void)
{
	message_to_LCD1602();
	switch_dev_adr();
	CURRENT_DEVICE_TYPE = unknown;
	COUNT_NULL_PACK = 0;			//	 для rename
}



void give_cmd_ping_null (void)
{
	COUNT_NULL_PACK = 0;								//	сбросим кол-во полученных нулевых пакетов
	tx_pack[BYTE_COMMAND] = CMD_PING;					//	записываем команду - пинг
	tx_pack[BYTE_PARAMETER] = PRM_NULL;					//	записываем параметр команды - запрос нулевых пакетов от новых устройств
	TIME_FOR_TIME_OUT_ACK = delay_for_null_packs;
}


void give_cmd_ping_type (void)
{
	tx_pack[BYTE_COMMAND] = CMD_PING;					//	записываем команду - задать новый адрес
	tx_pack[BYTE_PARAMETER] = PRM_PING;					//	записываем параметр команды	- без параметра
	TIME_FOR_TIME_OUT_ACK = delay_for_cmd;
}

void give_cmd_rename (void)
{
	tx_pack[BYTE_COMMAND] = CMD_SET_ADR;				//	записываем команду - задать новый адрес
	tx_pack[BYTE_PARAMETER] = PRM_NULL;					//	записываем параметр команды	- без параметра

	CURRENT_DEVICE = free_address(CURRENT_DEVICE_TYPE);	//	новый адрес согласно типу устройства
	tx_pack[BYTE_DATA_OFFSET] = CURRENT_DEVICE;

//	tx_pack[BYTE_DATA_OFFSET] = free_address(CURRENT_DEVICE_TYPE);

	tx_pack[BYTE_LEN] = tx_pack[BYTE_LEN] + 1;			//	увеличиваем длину на 1 байт
	TIME_FOR_TIME_OUT_ACK = delay_for_cmd;
}

void give_cmd_reset (void)
{
	tx_pack[BYTE_COMMAND] = CMD_CONTROL;				//	записываем команду - управление
	tx_pack[BYTE_PARAMETER] = PRM_RESET;				//	записываем параметр команды	- перезагрузка устройства
	TIME_FOR_TIME_OUT_ACK = delay_for_cmd;
}


uint8_t check_number_null_packs (uint8_t cnt)
{
	if (cnt > 1)		{cnt = TOO_MUCH;}		//	если получили больше одного ответного нулевого пакета
	if (cnt < 1)		{cnt = TOO_LITTLE;}		//	если не получили ни одного ответного нулевого пакета
	if (cnt == 1)		{cnt = ONE;}			//	если получили ОДИН нулевой пакет
	
	return 	cnt;
}



void give_cmd_status (void)
{
	tx_pack[BYTE_COMMAND] = CMD_STATUS;			//	записываем команду
	tx_pack[BYTE_PARAMETER] = PRM_STATUS;		//	записываем параметр команды
	TIME_FOR_TIME_OUT_ACK = delay_for_cmd;
}

void give_cmd_list (void)
{
	tx_pack[BYTE_COMMAND] = CMD_STATUS;			//	записываем команду
	tx_pack[BYTE_PARAMETER] = PRM_LIST;			//	записываем параметр команды
	TIME_FOR_TIME_OUT_ACK = delay_for_cmd;
}


	
void Parking_Space(void)
{
	static uint8_t ATTEMPTS = 0;				//	попытки (для поиска новых устройств)
	
	creat_pack();								//	собираем основу пакета
	TIME_FOR_TIME_OUT_ACK = delay_for_cmd;		//	ставим время ACK

	switch(PARKING_STAGE)
	{
		case SEARCH:		//	этап поиска	(любого) устройства
		{
			ATTEMPTS++;
			switch (CURRENT_DEVICE)		//	какой адрес сейчас
			{
				case NONAME_DEVICE:
				{
					switch (check_number_null_packs(COUNT_NULL_PACK))
					{
						case TOO_LITTLE:
						{
							switch(ATTEMPTS)
							{
								case 1:		{give_cmd_ping_null();}						break;	//	запрашиваем нулевые пакеты
								case 2:		{give_cmd_reset();}							break;	//	отправляем команду перезагрузки всем новым устройствас
								default:	{PARKING_STAGE = RESTART; ATTEMPTS = 0;}	break;	//	переключаемся к обычному опросу
							}
						}
						break;
						
						case TOO_MUCH:
						{
							if (ATTEMPTS < 4)	{give_cmd_ping_null();}				//	продолжаем инициализировать отправку нулевый пакетов
							else				{PARKING_STAGE = RESTART; ATTEMPTS = 0;}
						}
						break;

						case ONE:
						{
							if (ATTEMPTS > 2)	{PARKING_STAGE = DETECTED;	ATTEMPTS = 0;}	//	>2 потому что из TOO_LITTLE при первом же получении пакета попытки не обнулятся
							else				{give_cmd_ping_null();}						//	контрольный опрос
						}
						break;
					}
				}
				break;
				
				default: {PARKING_STAGE = DETECTED;	ATTEMPTS = 0;} break;		//	считаем что все остальные устройства существуют уже
			}
		}
		break;
		
		
		case DETECTED:	//	Обнаружили устройство	(был отсев до одного нулевого пакета, либо перешли сюда при любом существующем уже устройству)
		{
			COUNT_NULL_PACK = 0;
			
			switch (CURRENT_DEVICE_TYPE)		//	какой типу устроства
			{
				case unknown:					//	незнаем
				{
					if (ATTEMPTS > 1)	{PARKING_STAGE = RESTART;	ATTEMPTS = 0;}
					else				{give_cmd_ping_type();}
				}
				break;
				
				default:	{PARKING_STAGE = DEFINED; ATTEMPTS = 0;}	break;	//	тип неизвестного устройства устройства определен
			}
		}
		break;	
		
		
		case DEFINED:		//	определен тип устройства
		{
			switch (CURRENT_DEVICE)		//	����� ����� � ���������� � ������� �� ��������
			{
				case NONAME_DEVICE:		{give_cmd_rename();}	break;
					
				default:				//	если мы знаем тип устройства с которым сейчас работаем
				{
					if (ATTEMPTS > 2)	{PARKING_STAGE = RESTART; ATTEMPTS = 0;}

#ifdef that_device_is_HEAD

					switch (CURRENT_DEVICE_TYPE)
					{
						case NODE:
						{
							// сначала должны запросить список живых устройств
							if(LIST_UPDATED == 0)	{give_cmd_list();}

							// затем должны запросить список статусов
							if(STATUSES_UPDATED == 0)	{give_cmd_status();}

							if (LIST_UPDATED && STATUSES_UPDATED)	{PARKING_STAGE = RESTART; ATTEMPTS = 0;}	//	если все обновили, то идем дальше
							else									{ATTEMPTS++;}								//	если за два сообщения не вышло, то всеравно идем дальше
						}
						break;

						case SENSOR:	{give_cmd_status();}			break;
						case SCREEN:	{prepare_message_to_screen();}	break;	// в дальнейшем убереться, т.к. только HEAD обновляет экраны
					}

#else
						
					switch (CURRENT_DEVICE_TYPE)
					{
						case NODE:		{/*give_cmd_status();*/}			break;	//	нас не интересуют статусы других узлов
						case SENSOR:	{give_cmd_status();}			break;
						case SCREEN:	{prepare_message_to_screen();}	break;	// в дальнейшем убереться, т.к. только HEAD обновляет экраны
					}

					PARKING_STAGE = RESTART; ATTEMPTS = 0;

#endif


				}
				break;
			}
		}
		break;
		
		case RESTART:
		{

#ifdef that_device_is_HEAD

			LIST_UPDATED = 0;
			STATUSES_UPDATED = 0;

#else
#endif

			choice_next_dev();
			if(CURRENT_DEVICE == max_end_device)	{PARKING_STAGE = CICLE_DONE;}
			else									{PARKING_STAGE = SEARCH;}
		}
		break;
		
		case CICLE_DONE:
		{
			// обновим данные на всех панелях

			PARKING_STAGE = SEARCH;
		}
		break;
	}
		if (tx_pack[BYTE_COMMAND]) 		{put_tx_pack();}
			else {/*put_byte_to_soft_uart(0xFA);*/}
}
				
					
					
					
void put_tx_pack (void)
{
	tx_pack[BYTE_COUNT_PACK] = ++CURRENT_COUNT_PACK;
	tx_pack[tx_pack[BYTE_LEN]-1] =	crc8(&tx_pack[0],tx_pack[BYTE_LEN]-1);		//	11/12 byte:	посчитать и записать crc в пакет

	put_string_UART2(&tx_pack[0], tx_pack[BYTE_LEN]);

//	TIME_FOR_TIME_OUT_ACK = 1000;

	SET_BIT(Parking_Space_STATUS, (1<<waiting_ACK));		//	поднимаем флаг - ожидание ACK
	RTOS_SetTask(time_out_ACK,TIME_FOR_TIME_OUT_ACK,0);		//	время на получение всех пакетов (настроить ограничение максимального времени ожидания на датчиках)
}




void Parking_Space_Init (void)
{
//	uart1_init (9600);
//	uart2_init (9600);
	//	говорим какого типа данный контроллер (переделать для изменения через команды)

	#ifdef that_device_is_HEAD
		STATUSES_UPDATED = 0;
		LIST_UPDATED = 0;
	#else
	#endif

		/*
		RS232_address = eeprom_read_byte(EEPROM_ADR); 				//	адрес в сети RS485 (основная рабочая сеть системы)
		if (RS232_address == NONAME_DEVICE)	{Timer_0_ini_CTC();}	// если адрес нулевой, то запустим таймер для случайного значения
		RS232_ignor = 0;
		*/

		adr_in_uart_1 = 7;								//	пока дадим себе адрес №9 в сети верхнего уровня
		adr_in_uart_2 = 1;

		CURRENT_DEVICE = 2;								//	начнем с устройства №2
		COUNT_NULL_PACK = TOO_LITTLE;					//	чтобы запускался отсев

		Parking_Space_STATUS = 0;
		Parking_Space_CONTROL = 0;

		SET_BIT(Parking_Space_STATUS, (1<<Parking_Space_AUTO));

		PARKING_STAGE = SEARCH;
}



uint8_t sensor_is_live(uint8_t sens_adr)
{
	// Проверяем как помечен датчик с данным адресом (работает/неработает)
	uint8_t byte = sens_adr/8;						//	узнаем в каком байте лежит его состояние
	uint8_t bit = 7 - (sens_adr - (byte*8));	//	узнаем в каком бите лежит его состояние и зеркалим последовательность бит

	//	если датчик с этим имеет состояние "живой"
	if (READ_BIT(devices_is_live[byte],(1<<bit)))	{return 1;}
	else											{return 0;}
}

uint8_t sensor_is_free(uint16_t sens_adr)
{
	// Проверяем как помечен датчик с данным адресом (работает/неработает)
	uint8_t byte = sens_adr/8;					//	узнаем в каком байте лежит его состояние
	uint8_t bit = 7 - (sens_adr - (byte*8));	//	узнаем в каком бите лежит его состояние

	//	если место под этим датчиком "занято"
	if (READ_BIT(sensors_status[byte],(1<<bit)))	{return 0;}
	else											{return 1;}
}

void set_device_as_live(uint16_t _sensor)
{
	// пометить последний датчик, с которым работали, как живой
	uint16_t byte = _sensor/8;					//	узнаем в каком байте лежит его состояние
	uint16_t bit = 7 - (_sensor - (byte*8));		//	узнаем в каком бите лежит его состояние
	SET_BIT(devices_is_live[byte],(1<<bit));			//	записываем 1 в бите его состояния в массиве состояний датчиков (рабочий/нерабочий)
}

void set_device_as_dead(uint16_t _sensor)
{
	// пометить последний датчик, с которым работали, как нерабочий
	uint16_t byte = _sensor/8;					//	узнаем в каком он байте
	uint16_t bit = 7 - (_sensor - (byte*8));		//	узнаем какой бит ему соответствует
	CLEAR_BIT(devices_is_live[byte],(1<<bit));			//	пометим датчик как нерабочий
}

void set_status_as_taken(uint16_t _sensor)
{
	// пометить последний датчик, с которым работали, как занятое место
	uint16_t byte = _sensor/8;					//	узнаем в каком он байте
	uint16_t bit = 7 - (_sensor - (byte*8));	//	узнаем какой бит ему соответствует
	SET_BIT(sensors_status[byte],(1<<bit));				//	записываем 1 в бите его состояния в массиве состояний датчиков (рабочий/нерабочий)
}

void set_status_as_free(uint16_t _sensor)
{
	// пометить последний датчик, с которым работали, как свободное место
	uint16_t byte = _sensor/8;					//	узнаем в каком он байте
	uint16_t bit = 7 - (_sensor - (byte*8));		//	узнаем какой бит ему соответствует
	CLEAR_BIT(sensors_status[byte],(1<<bit));	//	записываем 0 в бите его состояния в массиве состояний датчиков (рабочий/нерабочий)
}
/////////	КОНЕЦ СЛУЖЕБНЫХ ФУНКЦИИ		/////////



void trigger (void)	//	входное значение - желаемое действие
{
	__disable_irq (); // запретить прерывания
	uint8_t STATUS = Parking_Space_STATUS;
	__enable_irq ();  // разрешить прерывания

	static uint8_t check_cmd = 1;
	if (check_cmd)		{check_cmd = 0;		SET_BIT (Parking_Space_STATUS,	(1<<check_CMD));}
	else				{check_cmd = 1;}

	if (READ_BIT(STATUS,(1<<check_CMD)))			{Parking_Space_CONTROL = DO_PARSING_CMD;	return;}
	if (READ_BIT(STATUS,(1<<CMD_ready)))			{Parking_Space_CONTROL = DO_CMD_EXE;		return;}
	if (READ_BIT(STATUS,(1<<ACK_ready)))			{Parking_Space_CONTROL = DO_ACK_EXE;		return;}
	if (READ_BIT(STATUS,(1<<waiting_ACK)))			{Parking_Space_CONTROL = DO_PARSING_ACK;	return;}

	if (READ_BIT(STATUS,(1<<Parking_Space_AUTO))) 	{Parking_Space_CONTROL = DO_PARKING_SPACE;}

}


void message_to_LCD1602 (void)
{

	uint8_t	count_live_devices = 0;			uint8_t	count_free_places = 0;
	char	result_live[1];					char	result_free[1];

	for (uint8_t i = 2; i < MAX_DEVICES; i++)
	{
	//	if (i != CURRENT_DEVICE)							//	проходим по все устройствам из памяти кроме текущего, т.к. текущий - экран
		{
			if (sensor_is_live(i))							//	проверяем статус жизни данного устройства
			{
				count_live_devices++;						//	сколько всего устройств живы в данный момент
				count_free_places += sensor_is_free(i);		//	сколько из них свободны
			}
		}
	}


	utoa(count_live_devices, 	result_live, 	10);
	utoa(count_free_places, 	result_free, 	10);		//	берет значение из переменной count_free_places, приводит его в десятичный вид и записывает в строку res_free

	char result_all[6];


	result_all[0] = result_free[0];	// res_free[0];	//	кол-во свободных мест с живых датчиков
	result_all[1] = '/';
	result_all[2] = result_live[0];	//	кол-во живый датчиков

	for (uint8_t i = 3; i < 6; i++)	{result_all[i] = 0x00;}	//	кастыль для подчистки хвоста строки

	LCD_Command(0x01);		//	очистка дисплея					(LCD_CLEAR)
	delay_ms(2);			//	долгая операция
//	LCD_Command(LCD_SETDDRAMADDR | SECONDSTRING);	//	писать с нулевого адреса
//	LCDsendString(result_all);

	//= выводим на превой строке байт с живыми устройствами ==================================================
	char string_lives[16];
	char string_statuses[16];

	string_lives[0] = 'L';
	string_lives[1] = 'i';
	string_lives[2] = 'v';
	string_lives[3] = 'e';
	string_lives[4] = 's';
	string_lives[5] = ':';
	string_lives[6] = ' ';
	string_lives[7] = ' ';

	for (int i = 7; i >= 0; i--)
	{
		if( READ_BIT(devices_is_live[0], (1<<i)) )	{string_lives[(7-i)+8] = '1';}
		else										{string_lives[(7-i)+8] = '0';}
	}

//	for (uint8_t i = 8; i < 16; i++)	{string_lives[i] = 0x00;}	//	кастыль для подчистки хвоста строки


	string_lives[8] = 'x';
	string_lives[9] = 'x';

	LCD_Command(LCD_SETDDRAMADDR);	//	писать с нулевого адреса
	LCDsendString(string_lives);

	//= выводим на второй строке байт со статусами устройств ==================================================

	string_statuses[0] = 'S';
	string_statuses[1] = 't';
	string_statuses[2] = 'a';
	string_statuses[3] = 't';
	string_statuses[4] = 'u';
	string_statuses[5] = 's';
	string_statuses[6] = ':';
	string_statuses[7] = ' ';

	for (int i = 7; i >= 0; i--)
	{
		if( READ_BIT(sensors_status[0], (1<<i)) )	{string_statuses[(7-i)+8] = '1';}
		else										{string_statuses[(7-i)+8] = '0';}
	}
//	for (uint8_t i = 8; i < 16; i++)	{string_statuses[i] = 0x00;}	//	кастыль для подчистки хвоста строки
/*
	string_statuses[8] = ' ';
	string_statuses[9] = ' ';
	string_statuses[10] = ' ';
	string_statuses[11] = ' ';
	string_statuses[12] = ' ';
	string_statuses[13] = result_all[0];
	string_statuses[14] = '/';
	string_statuses[15] = result_all[2];
*/

	string_statuses[8] = 'x';
	string_statuses[9] = 'x';

	LCD_Command(LCD_SETDDRAMADDR | SECONDSTRING);	//	писать с нулевого адреса
	LCDsendString(string_statuses);
	//=========================================================================================================
}


#ifdef that_device_is_HEAD

uint8_t rewrite_panels (void)
{
	uint8_t 	all_panels_is_rewrited = 0;

	if (all_panels_is_rewrited)			{return 1;}

	//	нужно отправить информацию на панель
	//	для этого нужно знать следующее:

	//	PANEL_global_N 		- глобальный номер панели на котору нужно отправить данные
	//	PANEL_local_adr		- локальные адрес панели в сети контроллера (узла)
	//	NODE_global_N 		- глобальный номер контроллера к которому подключена панель
	//	NODE_local_adr 		- локальные адрес контроллера к которому подключена панель
	//	GROUP				- группа котороая должны отображаться на панели
	//	RESULT				- количество свободных мест

	//	переменные, значения которых будут менять в процессе вычисления результата

	//	SUBGROUP 			- номер подгруппы, входящей в группу
	//	FIRST_SENSOR		- номер первого датчика в группе
	//	LAST_SENSOR			- номер последнего датчика в группе



	static 	uint8_t 	PANEL_global_N = 0;
			uint8_t 	PANEL_local_adr = 0;

	static 	uint8_t 	NODE_global_N = 0;
			uint8_t 	NODE_local_adr = 0;

	static	uint8_t 	GROUP = 0;

	static	uint16_t 	RESULT = 0;


			uint8_t 	SUBGROUP = 0;
			uint16_t 	FIRST_SENSOR_N = 0;
			uint16_t 	LAST_SENSOR_N = 0;



//=============	1. выгружаем список из памяти FLASH	=========================================================================================
	Page[WORD_CNT];					//	 массив - страницa в которую мы запишем списки, чтобы не выкачивать их каждый раз из памяти
	//	считываем страницу
	for (uint16_t i = 0; i < WORD_CNT; i++)		{Page[i] = read_word(LISTs + (i * WORD_ADDR_OFFSET));}	//	считываем по словам

//	Разделяем страницу на нужные нам списки

	uint32_t 	*SUBGROUP_2_SENSORS = 	&Page[SUBGROUP_2_SENSORS_list];		//	как сгруппированы датчики в подгруппы
	uint8_t 	*SUBGROUPS_2_GROUP = 	&Page[SUBGROUPS_2_GROUP_list];		//	как сгруппированиы подгруппы в группы
	uint8_t 	*PANELS_2_GROUP = 		&Page[PANELS_2_GROUP_list];			//	на какие панели закреплены группы
	uint8_t 	*PANELS_2_CONTROLLER = 	&Page[PANELS_2_CONTROLLER_list];	//	к каким контроллерам подключены панели
	uint8_t 	*PANEL_2_LOCAL_ADDR = 	&Page[PANEL_2_LOCAL_ADDR_list];		//	локальные адреса панелей в сети собственных контроллеров

//======================================================================================================================================================

// ищем в списке PANELS_2_GROUP панель, которая имеет группу

//=============	2. найдем группу и панель	=========================================================================================

	uint8_t find_panel_with_group = 0;

	while (!find_panel_with_group)
	{
		PANEL_global_N++;
		if (PANELS_2_GROUP[PANEL_global_N]	!=	0xFF)	//	если PANEL_i привязана к группе
		{
			GROUP = PANELS_2_GROUP[PANEL_global_N];	//	запомним номер группы
			find_panel_with_group = 1;
		}

		if(PANEL_global_N >= panels) {all_panels_is_rewrited = 1;}
	}

//======================================================================================================================================================

// теперь у нас есть панель и номер группы, который результат которой мы должны вывести

//=============	3. посчитаем состояния мест =========================================================================================

	uint8_t the_result_is_calculated = 0;

	while (!the_result_is_calculated)
	{
		SUBGROUP++;
		if (SUBGROUPS_2_GROUP[SUBGROUP]	==	GROUP)	//	если нашли подгруппу входящую в нашу группу)
		{
			// определяем с какого по какой датчик находятся в подгруппе
			FIRST_SENSOR_N = 0;
			LAST_SENSOR_N = 0;

			uint32_t content =  SUBGROUP_2_SENSORS[SUBGROUP];

			FIRST_SENSOR_N = 		(content >> 8) 	&& 0xFF;
			LAST_SENSOR_N = 		 content 		&& 0xFF;

			for (uint16_t sensor_N = FIRST_SENSOR_N; sensor_N <= LAST_SENSOR_N; sensor_N++)
			{
				RESULT += sensor_is_free(sensor_N);
			}
		}

		if(SUBGROUP >= subgroups) {the_result_is_calculated = 1;}
	}

//======================================================================================================================================================

// теперь у нас есть состояние всех датчиков входящий в нашу группу

//============ 4. Узнаем к какому контроллеру подключена панель и какой у панели локальный адрес =======================================================================================================

	NODE_global_N = PANELS_2_CONTROLLER[PANEL_global_N];
	NODE_local_adr = NODE_global_N;

	PANEL_local_adr = PANEL_2_LOCAL_ADDR[PANEL_global_N];

//=======================================================================================================================================

	// теперь у нас есть локальный адрес контроллера, локальный адрес панели, и результат

//============ 5. собираем пакет и отправляем =======================================================================================================

	tx_pack[BYTE_LEN] += 1;
	tx_pack[BYTE_DATA_OFFSET] = RESULT;		// пока так. Сами в терминале для начала посмотрим

	put_tx_pack();

return 0;

}
#else
#endif

