/**
  ******************************************************************************
  * @file    main.c
  * @author  Auto-generated by STM32CubeIDE
  * @version V1.0
  * @brief   Default main function.
  ******************************************************************************
*/


#include <global_defines.h>
#include <delay_ms.h>

#include <uart_1.h>
#include <uart_1_parsing.h>

#include <uart_2.h>
#include <uart_2_parsing.h>

#include <uart_3.h>
#include <lcd.h>
#include <rtos.h>
#include <mirf.h>

#include "protocol.h"
#include "parking_space.h"
#include "cmd_interp.h"

#include "flash.h"


void RCC_DeInit(void);			//	сбрасывает настройки тактирования
void SetSysClockTo72(void);		//	настраивает новое тактирование
void GPIO_Init (void);			//	настраивает входы выходы


void write_INIT_RTOS_in_lcd(void);
void send_byte_to_uart(void);

void system_reset(void);

void write_flash_HEAD (void);


#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

int main(void)
{

	RCC_DeInit();		//	сбрасываем тактирование
	SetSysClockTo72();	//	тактирование от внешнего 8 MHz -> 72 MHz
	GPIO_Init();		//	настройка портов
	SysTick_Init();		//	запуск системного таймера (для функции delay_ms())
	LCD_init();
	uart1_init (4800);
	uart2_init (4800);
//	uart3_init (9600);

	FLASH_Init();

	RTOS_Init();						//	запускает RTOS
//	RTOS_SetTask(write_INIT_RTOS_in_lcd, 3000, 0);		// для теста (через ~10 секунд включится светодиод на отладочной плате)
//	RTOS_DeleteTask(write_INIT_RTOS_in_lcd);

//	NRF_Init();




/*
//	Для светодиода на плате
	//============== Настройка вывода TX (PA9) ==================================================================================
		uint8_t offset = ( 9 - 8 ) * 4;				//	(9-8) * 4 = 4
		GPIOA->CRH &= ~( GPIO_BITS_MASK << offset );	//
		GPIOA->CRH |= ( OUTPUT_PUSH_PULL << offset );		//	альтернативная функция с выходом пуш-пул	AF_PUSH_PULL
	//===========================================================================================================================
*/

	//============== Настройка вывода TX (PB10) =================================================================================
		uint8_t offset =  (USART3_TX_pin - 8) * 4;				//	(10-8) * 4 = 8
		GPIOB->CRH &= ~( GPIO_BITS_MASK << offset );	//
		GPIOB->CRH |= ( OUTPUT_PUSH_PULL << offset );		//	альтернативная функция с выходом пуш-пул	AF_PUSH_PULL
	//===========================================================================================================================

//	RTOS_SetTask(send_byte_to_uart, 1000, 0);

	Parking_Space_Init();	//	инициализируем функции системы Parking_Space

/*
			uint32_t tmp_page[20];
			for (uint8_t i = 0; i < 10; i++)
			{
				tmp_page[i] = read_word ( Page_31_ADDR + (i*4) );

				uint8_t pack[4];
				pack[3] = tmp_page[i];
				pack[2] = tmp_page[i] >> (8 * 1);
				pack[1] = tmp_page[i] >> (8 * 2);
				pack[0] = tmp_page[i] >> (8 * 3);

				for (uint8_t j = 0; j < 4; j++)		{put_byte_UART1(pack[j]);}
				delay_ms(100);
			}

			uint8_t *byte = &tmp_page[0];
			for (uint8_t i = 0; i < 20; i++)
			{
				put_byte_UART1(byte[i]);
				delay_ms(100);
			}
*/

/*
	FLASH_Unlock();								//	разблокируем память
	FLASH_Erase_Page((uint32_t)LISTs);			//	стираем память
	FLASH_Lock();								//	блокируем память
*/




	uint8_t test_pack[1024];
	for (uint16_t  i = 0; i < 1024; i++)	{test_pack[i] = i;}

	FLASH_Unlock();								//	разблокируем память
//	FLASH_Erase_Page((uint32_t)LISTs);			//	стираем память
//	write_Page((uint32_t)(LISTs), &test_pack[0]);	//	пишем слово в пишем в следующие 32 бита (4 байта) (max 55)
	FLASH_Lock();								//	блокируем память



#ifdef that_device_is_HEAD



			uint32_t tmp_lists[32];
			for (uint8_t i = 0; i < 0xFF; i++)		//	т.к. по 4 байты выводим то код-во циклов должно быть 255
			{
				tmp_lists[i] = read_word ( LISTs + (i*4) );

				uint8_t pack[4];
				pack[0] = tmp_lists[i];
				pack[1] = tmp_lists[i] >> (8 * 1);
				pack[2] = tmp_lists[i] >> (8 * 2);
				pack[3] = tmp_lists[i] >> (8 * 3);

				static cnt_bytes = 0;
				for (uint8_t j = 0; j < 4; j++)		{put_byte_UART1(pack[j]); cnt_bytes++;}
				if(cnt_bytes == 16)					{delay_ms(100); cnt_bytes = 0;}
			}

			//	write_flash_HEAD();
			//	send_byte_to_uart();
#else
#endif
/*
			uint8_t *byte = &tmp_lists[0];
			for(uint8_t i = 0; i < 32; i++)
			{
				for (uint16_t j = 0; j < 32; j++)		{put_byte_UART1(byte[j]);}
			}
*/

/*
	for (uint8_t i = 0; i < 32; i++)
	{
		uint32_t tmp_b = read_word(Page_31_ADDR + (i*4));	// adr + (32*i)

		uint8_t pack[4];
		pack[3] = tmp_b;
		pack[2] = tmp_b >> (8 * 1);
		pack[1] = tmp_b >> (8 * 2);
		pack[0] = tmp_b >> (8 * 3);

		for (uint8_t j = 0; j < 4; j++)		{put_byte_UART1(pack[j]);}

		delay_ms(100);
	}

*/
			uint8_t *byte;
	uint32_t  byte_tmp = read_word(Page_30_ADDR);
	byte = &byte_tmp;
	adr_in_uart_1 = byte[0];

/*
	delay_ms(100);
	put_byte_UART1(adr_in_uart_1);
	put_byte_UART2(adr_in_uart_2);
	put_byte_UART1(0x01);
	put_byte_UART2(0x02);
*/



	put_byte_UART1(adr_in_uart_1);



	while(1)
	{
		RTOS_DispatchTask();	// обязательно крутится тут (иначе поставленные задачи будут вызываться из прерывания RTOS_timer
		trigger();	//	функция управляющая системой Parking_Space
		switch (Parking_Space_CONTROL)
		{
			case DO_PARSING_CMD:	{find_pack_from_uart_1();}	break;	//	искать пакет от ПК
			case DO_CMD_EXE:		{pack_from_uart_1_exe();}	break;	//	выполнить пакет от ПК
			case DO_PARSING_ACK:	{find_pack_from_uart_2();}	break;	//	искать пакет от подчиненного
			case DO_ACK_EXE:		{pack_from_uart_2_exe();}	break;	//	выполнить пакет от подчиненого

			case DO_PARKING_SPACE:	{Parking_Space();}			break;
		}
	}
}


// для теста RTOS
void write_INIT_RTOS_in_lcd(void)
{
	LCD_Command(0x01);		//	очистка дисплея					(LCD_CLEAR)
	delay_ms(2);			//	долгая операция

	LCD_Command(LCD_SETDDRAMADDR | 0);	//	писать с нулевого адреса
	LCDsendString("INIT RTOS");
}





void RCC_DeInit(void)
{
	SET_BIT(RCC->CR, RCC_CR_HSION);							//	Включим для начала HSI (внутренний генератор 8 МГц)
	while(READ_BIT(RCC->CR, RCC_CR_HSIRDY == RESET)) {}		//	Дождёмся его стабилизации

	MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80U);				//	Сбросим калибровку

	CLEAR_REG(RCC->CFGR);									//	Полностью очистим конфигурационный регистр
	while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET) {}	//	Дождёмся очистку бита SWS

	CLEAR_BIT(RCC->CR, RCC_CR_PLLON);						//	Отключим PLL
	while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET) {}	//	Ждем отключения

	CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);		//	Выключим HSE и его детектор тактового сигнала
	while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET) {}	//	Ждем отключения

	CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);						//	Сбросим бит, разрешающий использование внешнего генератора

	SET_BIT(RCC->CSR, RCC_CSR_RMVF);						//	Сбросим флаги всех прерываний от RCC

	CLEAR_REG(RCC->CIR);									//	Также запретим все прерывания от RCC
}



void SetSysClockTo72(void)
{
  SET_BIT	(RCC->CR, RCC_CR_HSEON);						//	Включим наш HSE, дождавшись его стабилизации (HSI - вунтренняя RC цепочка))
  while(READ_BIT(RCC->CR, RCC_CR_HSERDY == RESET)) {}		//	Дождёмся его стабилизации

  CLEAR_BIT	(FLASH->ACR, FLASH_ACR_PRFTBE);					//	Disable the Prefetch Buffer
  SET_BIT	(FLASH->ACR, FLASH_ACR_PRFTBE);					//	Enable the Prefetch Buffer (так надо)

  MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_2);	//	выбираем максимальную задержку

  // AHB prescaler											//	0xxx: SYSCLK not divided
  CLEAR_BIT	(RCC->CFGR, RCC_CFGR_HPRE_0);
  CLEAR_BIT	(RCC->CFGR, RCC_CFGR_HPRE_1);
  CLEAR_BIT	(RCC->CFGR, RCC_CFGR_HPRE_2);
  CLEAR_BIT	(RCC->CFGR, RCC_CFGR_HPRE_3);

  // APB low-speed prescaler (APB1)							//	100: HCLK divided by 2
  CLEAR_BIT	(RCC->CFGR,   RCC_CFGR_PPRE1_0);
  CLEAR_BIT	(RCC->CFGR,   RCC_CFGR_PPRE1_1);
  SET_BIT	(RCC->CFGR,   RCC_CFGR_PPRE1_2);

  // APB high-speed prescaler (APB2)						//	0xx: HCLK not divided
  CLEAR_BIT	(RCC->CFGR,  RCC_CFGR_PPRE2_0);
  CLEAR_BIT	(RCC->CFGR,  RCC_CFGR_PPRE2_1);
  CLEAR_BIT	(RCC->CFGR,  RCC_CFGR_PPRE2_2);

  // PLL entry clock source
  SET_BIT	(RCC->CFGR,  RCC_CFGR_PLLSRC);					//	1: HSE oscillator clock selected as PLL input clock

  // HSE divider for PLL entry
  CLEAR_BIT	(RCC->CFGR,  RCC_CFGR_PLLXTPRE);				//	0: HSE clock not divided

  // PLL multiplication factor								//	0111: PLL input clock x 9
  SET_BIT	(RCC->CFGR,  RCC_CFGR_PLLMULL_0);
  SET_BIT	(RCC->CFGR,  RCC_CFGR_PLLMULL_1);
  SET_BIT	(RCC->CFGR,  RCC_CFGR_PLLMULL_2);
  CLEAR_BIT	(RCC->CFGR,  RCC_CFGR_PLLMULL_3);

  SET_BIT(RCC->CR, RCC_CR_PLLON);									//	Разрешим работу PLL
  while(READ_BIT(RCC->CR, RCC_CR_PLLRDY) != (RCC_CR_PLLRDY)) {}		//	Дождёмся его включения

  MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);				//	Выберем PLL в качестве источника системного тактирования
  while(READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}	//	ждем применения данного действия

}



void GPIO_Init (void)
{
	uint32_t tmpreg;	//	пока используется для задержки.
	(void) tmpreg;
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN);				//	Alternate function IO clock enable	(запуск тактирования для SWD отладчика
	tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN);	//	Delay after an RCC peripheral clock enabling

	CLEAR_BIT(AFIO->MAPR,AFIO_MAPR_SWJ_CFG);
	SET_BIT(AFIO->MAPR, AFIO_MAPR_SWJ_CFG_JTAGDISABLE);		//	NOJTAG: JTAG-DP Disabled and SW-DP Enabled	(для отладчика)

	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);				//	enable port A	(запуск тактирование порта A)
	tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);	//	Delay after an RCC peripheral clock enabling


	//======= LED PC13 =========================================================================
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
		uint8_t offset;
		uint8_t LED_pin = 13;
		offset = (LED_pin - 8) * 4;						//	0 * 4 = 16
		GPIOC->CRH &= ~( GPIO_BITS_MASK << offset );	//	стереть 4 бита
		GPIOC->CRH |= ( OUTPUT_PUSH_PULL << offset );	//	записать 4 бита
		GPIOC->BSRR = ( 1 << LED_pin );				//	установка линии в 1
		//GPIOC->BRR = ( 1 << LED_pin );				//	установка линии в 0
	//==========================================================================================

}
void send_byte_to_uart(void)
{
//	put_byte_UART1(0xD1);
//	put_byte_UART1(Parking_Space_CONTROL);
//	put_byte_UART1(Parking_Space_STATUS);
//	put_byte_UART1(0xD1);

//	put_byte_UART2(0xD2);
//		put_byte_UART3(0xD3);

	RTOS_SetTask(send_byte_to_uart, 1000, 0);

	put_byte_UART1(devices_is_live[0]);
	put_byte_UART1(devices_is_live[1]);
	put_byte_UART1(devices_is_live[2]);
	put_byte_UART1(devices_is_live[3]);

	put_byte_UART1(sensors_status[0]);
	put_byte_UART1(sensors_status[1]);
	put_byte_UART1(sensors_status[2]);
	put_byte_UART1(sensors_status[3]);



}

void write_flash_HEAD (void)
{
	uint8_t HEAD_memory[1024];
	for (uint16_t  i = 0; i < 1024; i++)	{HEAD_memory[i] = 0xFF;}


// запишем подгруппы
//===	подгруппа №0	====================================================
	// first_sensor
	HEAD_memory[SUBGROUP_2_SENSORS_list + 0] = 0xFF;
	HEAD_memory[SUBGROUP_2_SENSORS_list + 1] = 0xFF;
	// last_sensor
	HEAD_memory[SUBGROUP_2_SENSORS_list + 2] = 0xFF;
	HEAD_memory[SUBGROUP_2_SENSORS_list + 3] = 0xFF;
//===========================================================================
//===	подгруппа №1	=====================================================
	// first_sensor
	HEAD_memory[SUBGROUP_2_SENSORS_list + 4] = 0x00;
	HEAD_memory[SUBGROUP_2_SENSORS_list + 5] = 0x20;	//	dec: 32
	// last_sensor
	HEAD_memory[SUBGROUP_2_SENSORS_list + 6] = 0x00;
	HEAD_memory[SUBGROUP_2_SENSORS_list + 7] = 0x21;	//	dea: 33
//===========================================================================
//===	подгруппа №2	=====================================================
	// first_sensor
	HEAD_memory[SUBGROUP_2_SENSORS_list + 8] = 0x00;
	HEAD_memory[SUBGROUP_2_SENSORS_list + 9] = 0x16;	//	dec: 22
	// last_sensor
	HEAD_memory[SUBGROUP_2_SENSORS_list + 10] = 0x00;
	HEAD_memory[SUBGROUP_2_SENSORS_list + 11] = 0x16;	//	dea: 22
//===========================================================================
//===	подгруппа №3	=====================================================
	// first_sensor
	HEAD_memory[SUBGROUP_2_SENSORS_list + 12] = 0x00;
	HEAD_memory[SUBGROUP_2_SENSORS_list + 13] = 0x17;	//	dec: 23
	// last_sensor
	HEAD_memory[SUBGROUP_2_SENSORS_list + 14] = 0x00;
	HEAD_memory[SUBGROUP_2_SENSORS_list + 15] = 0x17;	//	dea: 23
//===========================================================================
//===	подгруппа №4	=====================================================
	// first_sensor
	HEAD_memory[SUBGROUP_2_SENSORS_list + 16] = 0x00;
	HEAD_memory[SUBGROUP_2_SENSORS_list + 17] = 0x20;	//	dec: 32
	// last_sensor
	HEAD_memory[SUBGROUP_2_SENSORS_list + 18] = 0x00;
	HEAD_memory[SUBGROUP_2_SENSORS_list + 19] = 0x21;	//	dea: 33
//===========================================================================
//===	подгруппа №5	=====================================================
	// first_sensor
	HEAD_memory[SUBGROUP_2_SENSORS_list + 20] = 0x00;
	HEAD_memory[SUBGROUP_2_SENSORS_list + 21] = 0x0D;	//	dec: 13
	// last_sensor
	HEAD_memory[SUBGROUP_2_SENSORS_list + 22] = 0x00;
	HEAD_memory[SUBGROUP_2_SENSORS_list + 23] = 0x0D;	//	dec: 13
//===========================================================================
//===	подгруппа №6	=====================================================
	// first_sensor
	HEAD_memory[SUBGROUP_2_SENSORS_list + 24] = 0x00;
	HEAD_memory[SUBGROUP_2_SENSORS_list + 25] = 0x16;	//	dec: 22
	// last_sensor
	HEAD_memory[SUBGROUP_2_SENSORS_list + 26] = 0x00;
	HEAD_memory[SUBGROUP_2_SENSORS_list + 27] = 0x17;	//	dec: 23
//===========================================================================
//===	подгруппа №7	=====================================================
	// first_sensor
	HEAD_memory[SUBGROUP_2_SENSORS_list + 28] = 0x00;
	HEAD_memory[SUBGROUP_2_SENSORS_list + 29] = 0x20;	//	dec: 32
	// last_sensor
	HEAD_memory[SUBGROUP_2_SENSORS_list + 30] = 0x00;
	HEAD_memory[SUBGROUP_2_SENSORS_list + 31] = 0x20;	//	dec: 32
//===========================================================================


// покажем подгруппам в каких группах они находяться
//===	подгруппа №0 никуда не входит	==========================================
	HEAD_memory[SUBGROUPS_2_GROUP_list + 0] = 0xFF;		// sub_grioup_0 -> X
//================================================================================
//===	подгруппа №1 входит в группу №1	==========================================
	HEAD_memory[SUBGROUPS_2_GROUP_list + 1] = 0x01;		// sub_grioup_1 -> group_1
//================================================================================
//===	подгруппа №2 входит в группу №1	==========================================
	HEAD_memory[SUBGROUPS_2_GROUP_list + 2] = 0x01;		// sub_grioup_2 -> group_1
//================================================================================
//===	подгруппа №3 входит в группу №2	==========================================
	HEAD_memory[SUBGROUPS_2_GROUP_list + 3] = 0x02;		// sub_grioup_3 -> group_2
//================================================================================
//===	подгруппа №4 входит в группу №2	==========================================
	HEAD_memory[SUBGROUPS_2_GROUP_list + 4] = 0x02;		// sub_grioup_4 -> group_2
//================================================================================
//===	подгруппа №5 входит в группу №3	==========================================
	HEAD_memory[SUBGROUPS_2_GROUP_list + 5] = 0x03;		// sub_grioup_5 -> group_3
//================================================================================
//===	подгруппа №6 входит в группу №3	==========================================
	HEAD_memory[SUBGROUPS_2_GROUP_list + 6] = 0x03;		// sub_grioup_6 -> group_3
//================================================================================
//===	подгруппа №7 входит в группу №3	==========================================
	HEAD_memory[SUBGROUPS_2_GROUP_list + 7] = 0x03;		// sub_grioup_7 -> group_3
//================================================================================


// Закрепим панели за группами
//===	панель №0 не выводит никакую группу	======================================
	HEAD_memory[PANELS_2_GROUP_list + 0] = 0xFF;		// panel_0 <- X
//================================================================================
//===	панель №1 выводит группу №3	==============================================
	HEAD_memory[PANELS_2_GROUP_list + 1] = 0x03;		// panel_1 <- group_3
//================================================================================
//===	панель №2 выводит группу №1	==============================================
	HEAD_memory[PANELS_2_GROUP_list + 2] = 0x01;		// panel_2 <- group_1
//================================================================================
//===	панель №3 выводит группу №2	==============================================
	HEAD_memory[PANELS_2_GROUP_list + 3] = 0x02;		// panel_3 <- group_2
//================================================================================
//===	панель №4 выводит группу №2	==============================================
	HEAD_memory[PANELS_2_GROUP_list + 4] = 0x02;		// panel_4 <- group_2
//================================================================================


// укажем к какому контроллеру подключена панель
//===	панель №0 не выводит никакую группу	======================================
	HEAD_memory[PANELS_2_CONTROLLER_list + 0] = 0xFF;		// panel_0 -> X
//================================================================================
//===	панель №1 не выводит никакую группу	======================================
	HEAD_memory[PANELS_2_CONTROLLER_list + 1] = 0x01;		// panel_1 -> head
//================================================================================
//===	панель №2 не выводит никакую группу	======================================
	HEAD_memory[PANELS_2_CONTROLLER_list + 2] = 0x02;		// panel_2 -> node_2
//================================================================================
//===	панель №3 не выводит никакую группу	======================================
	HEAD_memory[PANELS_2_CONTROLLER_list + 3] = 0x03;		// panel_3 -> node_3
//================================================================================
//===	панель №4 не выводит никакую группу	======================================
	HEAD_memory[PANELS_2_CONTROLLER_list + 4] = 0x04;		// panel_4 -> node_4
//================================================================================


// укажем локальные адреса панелей
//===	панель №0 не существует	==================================================
	HEAD_memory[PANEL_2_LOCAL_ADDR_list + 0] = 0xFF;		// panel_0 = X
//================================================================================
//===	панель №1 иммет локальный адрес 0x05	==================================
	HEAD_memory[PANEL_2_LOCAL_ADDR_list + 1] = 0x05;		// panel_1 = 0x05
//================================================================================
//===	панель №2 иммет локальный адрес 0x04	==================================
	HEAD_memory[PANEL_2_LOCAL_ADDR_list + 2] = 0x04;		// panel_2 = 0x04
//================================================================================
//===	панель №3 иммет локальный адрес 0x04	==================================
	HEAD_memory[PANEL_2_LOCAL_ADDR_list + 3] = 0x04;		// panel_3 = 0x04
//================================================================================
//===	панель №4 иммет локальный адрес 0x04	==================================
	HEAD_memory[PANEL_2_LOCAL_ADDR_list + 4] = 0x04;		// panel_4 = 0x04
//================================================================================


	FLASH_Unlock();									//	разблокируем память
	FLASH_Erase_Page((uint32_t)LISTs);				//	стираем память
	write_Page((uint32_t)(LISTs), &HEAD_memory[0]);	//	пишем слово в пишем в следующие 32 бита (4 байта) (max 55)
	FLASH_Lock();									//	блокируем память

}




