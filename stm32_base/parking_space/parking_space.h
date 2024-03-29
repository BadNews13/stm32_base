/*
 * polling.h
 *
 * Created: 25.09.2020 12:19:11
 *  Author: bad_n
 */ 


#ifndef POLLING_H_
#define POLLING_H_

	 #include "global_defines.h"
	 #include "protocol.h"
 	 #include "flash.h"


// из parking_defines.h ======================================================================

//#define that_device_is_HEAD						//	влияет на выделение памяти и обработку некоторых ACK ответов

#define MAX_DEVICES 	10							//	количество устройств в данном узле (и во всех остальный узлах по столько же)
#define OVER			21						//	максимальное количество попыток ATTEMPTS

#ifdef that_device_is_HEAD

	uint8_t devices_is_live[	((MAX_DEVICES * MAX_DEVICES) / 8 ) + 1	];	//	массив работающих устройств (1 бит - 1 устройство) (номер бита в массиве = адресу датчика)
	uint8_t sensors_status[		((MAX_DEVICES * MAX_DEVICES) / 8 ) + 1	];
	uint8_t dev_type[2][		((MAX_DEVICES * MAX_DEVICES) / 8 ) + 1	];

	#define node_offset	MAX_DEVICES				//	отступ в нашем массиве для каждого следующего контроллера

	volatile uint8_t LIST_UPDATED;				//	список живых устройств обновлен

#else

	uint8_t devices_is_live[(MAX_DEVICES/8)+1];		//	массив работающих устройств (1 бит - 1 устройство) (номер бита в массиве = адресу датчика)
	uint8_t sensors_status[(MAX_DEVICES/8)+1];		//	массив результатов датчиков (1 бит - 1 датчик) (номер бита в массиве = адресу датчика)
	uint8_t dev_type[2][(MAX_DEVICES/8)+1];

#endif
	volatile uint8_t STATUS_UPDATED;			//	список статусов обновлен

//====== для dev_type ================================================================================================================

	#define bit_0 0
	#define bit_1 1
	//	bit_0	bit_1
	// 	0		0 		- UNKNOWN
	// 	0		1 		- NODE
	// 	1		0	 	- SENSOR
	// 	1		1		- PANEL

//==========================================================================================================================



//метки сетей
uint8_t used_network;
#define MK_FREE		0x00
#define RS232		0x01
#define RS485		0x02
#define MIRF		0x03
#define OUTPUT		0x03

#define UART_1		0x01
#define UART_2		0x02

//==============================================================================================



	void Parking_Space_Init (void);
	void Parking_Space (void);
	void trigger (void);
	void message_to_LCD1602 (void);

	volatile uint8_t CURRENT_ACTION;

	void time_out_ACK (void);
	void creat_pack (void);
	void switch_dev_adr (void);
	void give_cmd_ping_type (void);
	void give_addess (uint8_t dev_type);

	void rebuild_for_resend (uint8_t *pack);

	uint8_t free_address (uint8_t type_dev);


	uint8_t tx_pack[MAX_PACK_LENGTH];		//	пакет исходящий в подстеть
	uint8_t rx_pack[MAX_PACK_LENGTH];		//	пакет приходящий из подсети


	volatile uint8_t Parking_Space_CONTROL;
				#define		DO_PARSING_CMD			1	// 	Действие: искать команду
				#define		DO_CMD_EXE				2	// 	Действие: обработать команду
				#define		DO_PARSING_ACK			3	// 	Действие: искать ответ
				#define		DO_ACK_EXE				4	// 	Действие: обработать ответ
				#define		DO_PARKING_SPACE		5	// 	Действие: работа системы навигации


	volatile uint8_t Parking_Space_STATUS;
				#define		Parking_Space_AUTO		0	//	Запустить автоновную работу системы
				#define		check_CMD				1	//	Статус: необходимо проверить команду
				#define		CMD_ready				2	//	Статус: ожидание обработки команды
				#define		waiting_ACK				3	//	Статус: ожидание ответа
				#define		ACK_ready				4	//	Статус: ожидание обработки ответа

	// для выбора действия в этапе RENAME
			#define TOO_LITTLE	0					//	слишком мало устроств для переименования
			#define ONE			1					//	одно устройство готово к переименованию
			#define TOO_MUCH	2					//	слишком много устройст для переименования

	// задержки таймаута
			#define delay_for_cmd			400		//	ждем ответ на команду	100
			#define delay_for_measurement	800		//	ждем ответ на запрос измерения	200
			#define delay_for_null_packs	2000	//	ждем пока все нулевые пакеты будут отправлены в сеть	500

			#define	FULL_CMD_PING				0x2101		//	CMD_PING + PRM_PING
			#define	FULL_CMD_GET_LIST			0x2402		//	CMD_STATUS + PRM_LIST
			#define	FULL_CMD_GET_STATUS			0x2401		//	CMD_STATUS + PRM_STATUS
			#define	FULL_CMD_WRITE_TEXT			0x2C01		//	CMD_WRITE_TEXT + PRM_RED
			
			void prepare_message_to_screen (void);



	//////////////////////////////////////////////////
	volatile uint8_t PARKING_STAGE;

	// ������ � ������� Parking_Space (�������� �������� � ���������� PARKING_STAGE)
		#define	SEARCH				0x08			//	поиск (пинг) устроства
		#define	DETECTED			0x0C			//	устройство определен
		#define	DEFINED				0x0E
		#define	ASSIGNED			0x0F
		#define SWITCH				0x00
		#define CICLE_DONE			0x10
		
		#define IS_NONAME	0xFF

		uint8_t check_number_null_packs (uint8_t cnt);
	//	void parking_space (void);
		void put_tx_pack (void);
		void give_cmd_status (void);
		void give_cmd_list (void);
	/////////////////////////////////////////////////




	#ifdef that_device_is_HEAD
	
	// 1						- PC
	// 60						- HEADS
	// 60 * HEAD				- NODES
	// 60 * NODES				- SENSORS
	// 3 * (HEADS + NODES)		- SCREENS

	// Основная задача головного контроллера - вывести обработанные данные на панели



	//	subgroup	<=		sensors
	//	subgroups	=>		group
	//	panels		=>		group
	//	panels		=>		controller
	//	panel		<=>		panel_adr
	//	controller	<=>		controller_adr

	uint32_t Page[WORD_CNT];

	//адреса во FLASH памяти
	#define LISTs Page_31_ADDR			//	выделенная память во flash для хранения списков						//	адрес ячейки в энергонезависимой памяти где храним имя контроллера (подчиненное имя)

	// адреса началов списковВ EEPROM
	#define	SUBGROUP_2_SENSORS_list			0x0000		//	size: 4 byte	quantity: 100
	#define	SUBGROUPS_2_GROUP_list			0x0190		//	size: 1 byte	quantity: 128	//	есть лишние 28 байт
	#define	PANELS_2_GROUP_list				0x0210		//	size: 1 byte	quantity: 128	//
	#define	PANELS_2_CONTROLLER_list		0x0270		//	size: 1 byte	quantity: 128
	#define	PANEL_2_LOCAL_ADDR_list			0x02D0		//	size: 1 byte	quantity: 128
	#define	CONTROLLER_2_LOCAL_ADDR_list	0x0330		//	size: 1 byte	quantity: 128

	//	пересчитать количество
		#define panels			96
		#define subgroups		100
		#define controllers		208
		#define groups			100

	#else
	#endif


	//=========================================================
	uint8_t UPDATE_STAGE;

	#define UPDATE_START				0	//	начинаем
	#define GROUP_DETECTING				1	//	выбираем группу
	#define SUBGROUP_DETECTING			2	//	определяем подгруппу (заходим столько раз - сколько потребуется)
	#define STATUS_CALCULATING			3	//	считаем количество свободных мест в группе на основании количества свободных мест в подгруппе (заходим столько раз - сколько потребуется)
	#define PANEL_DETECTING				4	//	определяем панель на которую нужно вывести результат
	#define CONTROLLER_DETECTING		5	//	определяем панель на которую нужно вывести результат
	#define LOCAL_ADDRESSES_DETECTING	6	//	определение локального адреса контроллера и локального адреса панели (в подсети контроллера)
	#define INFO_UPDATING				7	//	отправка сообщения на панель
	#define UPDATE_FINISH				8	//	обновление информации на панелях произведено
	//=========================================================


	#define FIRST_SENSOR	0
	#define LAST_SENSOR		1
	#define EMPTY			0xFF





uint8_t device_is_live(uint16_t adr_sensor);		//	проверить живой ли сенсор по данному адресу
uint8_t sensor_is_free(uint16_t adr_sensor);		//	проверить свободно ли место по данному адресу

void set_device_as_live(uint16_t _sensor);		//	пометить сенсор как мертвый
void set_device_as_dead(uint16_t _sensor);		//	пометить сенсор как живой
void set_status_as_taken(uint16_t _sensor);		//	пометить последний датчик, с которым работали, как занятое мест
void set_status_as_free(uint16_t _sensor);		//	пометить последний датчик, с которым работали, как свободное место


uint8_t rewrite_panels (void);



		
#endif /* POLLING_H_ */
