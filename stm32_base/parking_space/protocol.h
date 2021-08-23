/*
 * protocol.h
 *
 * Created: 12.08.2020 11:09:23
 *  Author: bad_n
 */ 


#ifndef PROTOCOL_H_
#define PROTOCOL_H_



#define PACK_HEADER_LENGTH          8			//	длина заголовка пакета (после идет команда, параметр, данные и crc)
#define MAX_PACK_LENGTH             128
#define MIN_PACK_LENGTH             11
#define MAX_PACK_LENGTH_FOR_MIRF	12

//информация о пакете
#define BYTE_LEN					0			//	длина пакета
#define BYTE_RECEIVER_ADR			1			//	первый получатель
#define BYTE_NEXT_RECEIVER_ADR		2			//	второй получатель
#define BYTE_ADDR_3					3			//  символ конца пути (плавает в зависимости от количества передач (может быть на месте и второго получателя и вторго отправителя)
#define BYTE_PREVIOUS_SENDER_ADR	4			//	второй отправитель (отсчет от нас, т.е. это тот кто отправил нашему отправителю)
#define BYTE_SENDER_ADR				5			//	первый отправитель (отсчет от нас, т.е. это тот кто отправил нам)
#define BYTE_COUNT_PACK				6			//	счетчик сгенерированных пакетов (не увеличивается при пересылке)
#define BYTE_FLAGS					7			//	флаги пакета
#define BYTE_COMMAND				8			//	команда
#define BYTE_PARAMETER				9			//	параметр
#define BYTE_DATA_OFFSET			10			//	байт с которого начинаются данные
//

#define SEPARATOR					0xAA

//флаги в пакете
#define CMD_FLAGS_PACK				0			//	флаг "это пакет"
#define CMD_FLAGS_ACK_FLAG			1			//	флаг "это ответ"
#define CMD_FLAGS_ERROR				7			//	флаг "ошибка"


volatile uint8_t ADDR_OF_SELECTED_DEVICE;		//	устройство к которому был послан последний пакет
volatile uint8_t TYPE_OF_SELECTED_DEVICE;		//	устройство к которому был послан последний пакет
volatile uint8_t CURRENT_COUNT_PACK;			//	номер сгенерированного пакета


volatile uint8_t COUNT_NULL_PACK;			//	счетичк 3-х байтных пакетов от нулевых датчиков
volatile uint8_t MY_DEVICE_TYPE;


#define max_end_device			MAX_DEVICES		//	максимальное количество устройств


//	Типы устройств
#define I_am						MY_DEVICE_TYPE
#define HEAD						0xA0	//	главный контроллер либо ПК
#define NODE						0xA1	//	узловой контроллер
#define SENSOR						0xA2	//	датчик
#define SCREEN						0xA3	//	экран


//Спецсимфолы для пакета
#define CMD_IS_DONE					0x01	//	команда выполнена
#define ROUTE_END					0x00	//	символ конца маршрута
#define NO_PARAMETERS				0x00	//	Парамметры у команды отсутствуют
#define NO_DATA						0x00	//	данные отсутствуют
#define MASTER						0x01	//	адрес мастера
#define NEW_SENSOR					0x00	//	адрес нового датчика не получившего адрес в нашей сети
#define ERROR						0xAA	//	ошибка обработки (в будущем вписывать код ошибки)
//

// состояние парковочного места
#define unknown                     0x00        //  статус места "неизвестно"
#define TAKEN                       0x01        //  статус места "занято"
#define FREE                        0x02        //  статус места "свободно"
//


//полный список команд========================================
#define CMD_CONTROL					0x20			//	управление устройством
	#define PRM_START					0x00
	#define PRM_STOP					0x01
	#define PRM_RESET					0x02

#define CMD_PING					0x21			// команда: пинг устройства
	#define PRM_NULL					0x00			//  параметр: запросить статус датчика у самого датчика, либо запросить статусы датчиков из памяти контроллера
	#define PRM_PING					0x01			//  параметр: запрос типа устройства
	#define PRM_BLINK					0x02			//  параметр: помигать датчиком


#define CMD_SET_ADR					0x23			//	команда: установить новый адрес

#define CMD_STATUS					0x24			//	(param: STATUS(0x00) - list_statuses; LIST_DEVICES(0x01) - list_lives)
	#define PRM_DISTANCE				0x00			//	параметр: запросить измерение расстояния и отправку результата от датчика
	#define PRM_STATUS					0x01			//	параметр: запросить статус датчика у самого датчика, либо запросить статусы датчиков из памяти контроллера
	#define PRM_LIST					0x02			//	запросить список живыъ датчиков подключенных к контроллеру


#define CMD_REF_DISTANCE			0x25			//	команда: референтная дистанция
	#define PRM_SET						0x00			//	параметр: установить новую референтную дистанцию
	#define PRM_GET						0x01			//	параметр: получить текущую референтную дистанцию

//#define RESERVED					0x26			//	команда не используется

#define CMD_CREATE_GROUP				0x27		//	команда: Работа с группой (создание)
	#define PRM_SENSORS_TO_SUBGROUP			0x00		//	параметр: объединить датчики в подгруппу
	#define PRM_SUBGROUP_TO_GROUP			0x01		//	параметр: объединить подгруппу в группу
	#define PRM_PANEL_TO_GROUP				0x02		//	параметр: привязать панели к группе

/*
#define CMD_CLEAR_GROUP				0x28
	#define PRM_JOIN					0x00
	#define PRM_DETACH					0x01
*/
#define CMD_GROUP_STATUS			0x29
//	#define PRM_SENSORS_TO_SUBGROUP			0x00
//	#define PRM_SUBGROUP_TO_GROUP			0x01
//	#define PRM_PANEL_TO_GROUP				0x02
	#define PRM_GROUP_LIST					0x03
		


//#define RESERVED					0x2A			//	команда не используется
//#define RESERVED					0x2B			//	команда не используется

#define CMD_WRITE_TEXT				0x2C			//	(param: RED(0x01) - текст красного цвета)
	#define PRM_BLACK					0x01			//	параметр: текст черного цвета
	#define PRM_RED						0x01			//	параметр: текст красного цвета
	#define PRM_GREEN					0x02			//	параметр: текст зеленого цвета	//	проверить

#define CMD_DI_DO					0x2D			//	команда: работа с цифровыми входами-выходами устройства
	#define PRM_WRITE					0x00			//	параметр: установить новое состояние на цифровых выходах устройства
	#define PRM_READ					0x01			//	параметр: считать состояние на цифровых входах устройства

//#define RESERVED					0x2E			//	команда не используется

#define CMD_SET_MIRF_CHANEL			0x2F
//============================================================


//Параметры:
// для команды CMD_STATUS

#define LIST_STATUSES				0x01
#define LIST_DEVICES				0x02

#define PRM_STATUSES	PRM_STATUS

#define the_next_stage_is_needed	0xD0





#define subgroups		100
#define groups			100
#define panels			100
#define controllers		100		//	может быть как NODE, так и HEAD
#define sensors			999






#endif /* PROTOCOL_H_ */
