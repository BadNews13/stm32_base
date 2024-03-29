/*
 * lcd.c
 *
 *  Created on: Apr 15, 2021
 *      Author: bad_n
 */


#include "lcd.h"
#include "delay_ms.h"
#include <delay_us.h>

const uint8_t LcdCustomChar[] =	//define 8 custom LCD chars
{
	0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, // 0. 0/5 full progress block
	0x1F, 0x00, 0x10, 0x18, 0x18, 0x10, 0x00, 0x1f, // 1. 1/5 full progress block
	0x1F, 0x00, 0x18, 0x1c, 0x1c, 0x18, 0x00, 0x1f, // 2. 2/5 full progress block
	0x1F, 0x00, 0x1C, 0x1e, 0x1e, 0x1C, 0x00, 0x1f, // 3. 3/5 full progress block
	0x1F, 0x00, 0x1E, 0x1f, 0x1f, 0x1E, 0x00, 0x1f, // 4. 4/5 full progress block
	0x1F, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, 0x1f, // 5. 5/5 full progress block
	0x03, 0x04, 0x09, 0x13, 0x13, 0x09, 0x04, 0x03, // 6. rewind arrow
	0x18, 0x04,	0x12, 0x19,	0x19, 0x12, 0x04, 0x18  // 7. fast-forward arrow
};

//Таблица перекодировки в русские символы.
const char convert_HD44780[64] =
//static const unsigned char convert_HD44780[64] =
{
	0x41,0xA0,0x42,0xA1,0xE0,0x45,0xA3,0xA4,
	0xA5,0xA6,0x4B,0xA7,0x4D,0x48,0x4F,0xA8,
	0x50,0x43,0x54,0xA9,0xAA,0x58,0xE1,0xAB,
	0xAC,0xE2,0xAD,0xAE,0xAD,0xAF,0xB0,0xB1,
	0x61,0xB2,0xB3,0xB4,0xE3,0x65,0xB6,0xB7,
	0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0x6F,0xBE,
	0x70,0x63,0xBF,0x79,0xE4,0x78,0xE5,0xC0,
	0xC1,0xE6,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7
};


void LCD_init(void)
{

	timer_delay_us_ini();	//	позволяет использовать задержики в us

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

	uint8_t offset;

	uint8_t tmpreg;
	tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN);	//	Delay after an RCC peripheral clock enabling
	tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);	//	Delay after an RCC peripheral clock enabling


//============== REGISTER_SELECT (PB9) =======================================================================================
	offset = ( pin_RS - 8 ) * 4;						//	(9-8) * 4 = 4
	GPIOB->CRH &= ~( GPIO_BITS_MASK << offset );		//	стереть 4 бита // (0xF << 20) - (bit_23, bit_22, bit_21, bit_20)
	GPIOB->CRH |= ( OUTPUT_PUSH_PULL << offset );		//	записать 4 бита
	GPIOB->BSRR = ( 1 << pin_RS );
//============================================================================================================================

//============== READ_WRITE (PB8) ============================================================================================
	offset = ( pin_RW - 8 ) * 4;						//	(8-8) * 4 = 4
	GPIOB->CRH &= ~( GPIO_BITS_MASK << offset );		//	стереть 4 бита // (0xF << 20) - (bit_23, bit_22, bit_21, bit_20)
	GPIOB->CRH |= ( OUTPUT_PUSH_PULL << offset );		//	записать 4 бита
	GPIOB->BRR = ( 1 << pin_RW );
//============================================================================================================================

//============== ENABLED (PB7) ===============================================================================================
	offset = pin_E * 4;
	GPIOB->CRL &= ~( GPIO_BITS_MASK << offset );
	GPIOB->CRL |= ( OUTPUT_PUSH_PULL << offset );
	GPIOB->BRR = ( 1 << pin_E );
//============================================================================================================================



//============== Data_4 (PB6) =================================================================================================
	offset = pin_D4 * 4;
	GPIOB->CRL &= ~( GPIO_BITS_MASK << offset );
	GPIOB->CRL |= ( OUTPUT_PUSH_PULL << offset );
	GPIOB->BSRR = ( 1 << pin_D4 );
//============================================================================================================================

//============== Data_5 (PB12) ===============================================================================================
	offset = ( pin_D5 - 8 ) * 4;						//	(12-8) * 4 = 16
	GPIOB->CRH &= ~( GPIO_BITS_MASK << offset );		//	стереть 4 бита // (0xF << 20) - (bit_23, bit_22, bit_21, bit_20)
	GPIOB->CRH |= ( OUTPUT_PUSH_PULL << offset );		//	записать 4 бита
	GPIOB->BSRR = ( 1 << pin_D5 );
//============================================================================================================================

//============== Data_6 (PB4) ================================================================================================
	offset = pin_D6 * 4;
	GPIOB->CRL &= ~( GPIO_BITS_MASK << offset );
	GPIOB->CRL |= ( OUTPUT_PUSH_PULL << offset );
	GPIOB->BSRR = ( 1 << pin_D6 );
//============================================================================================================================

//============== Data_7 (PB3) ================================================================================================
	offset = pin_D7 * 4;
	GPIOB->CRL &= ~( GPIO_BITS_MASK << offset );
	GPIOB->CRL |= ( OUTPUT_PUSH_PULL << offset );
	GPIOB->BSRR = ( 1 << pin_D7 );
//============================================================================================================================



//============== BRIGHT (PA15) ===============================================================================================
	offset = ( pin_bright - 8 ) * 4;					//	(15-8) * 4 = 28
	GPIOA->CRH &= ~( GPIO_BITS_MASK << offset );		//	стереть 4 бита // (0xF << 20) - (bit_23, bit_22, bit_21, bit_20)
	GPIOA->CRH |= ( OUTPUT_PUSH_PULL << offset );		//	записать 4 бита
//============================================================================================================================



//============== запуск LCD ==================================================================================================
	LCD_WriteData(0xFF);
	delay_us(10);

	LCD_Send(0x03);
	LCD_Send(0x03);
	LCD_Send(0x02);

	LCD_WriteData(0xFF);
	delay_us(10);
//============================================================================================================================

/*
	LCD_Command(0x0F);//дисплей включаем (D=1), также включаем пока все курсоры
	LCD_Command(0x06);//пишем влево.
	LCD_Command(0x02);//возврат курсора в нулевое положение		(LCD_HOME)
*/
	LCD_Command(0x28);		//	режим 4 бит, 2 линии (для нашего большого дисплея это 4 линии), шрифт 5х8
	LCD_Command(0x0C);
	LCD_Command(0x01);		//	очистка дисплея					(LCD_CLEAR)
	delay_ms(2);			//	долгая операция

	LCD_Command(LCD_SETDDRAMADDR | 0);	//	писать с нулевого адреса
	LCDsendString("INIT DONE");

	bright_on;

/*
//============== Пример:  вывести с текущего адреса 16 символов (0123456789:;>=<?@) ===========================
	for (uint8_t i = 1; i<16; i++)	{LCD_Char(0x30 + i);}	//
//=============================================================================================================
*/

/*
//============== Пример: вывести "1" в первой строке в позиции 8 ==============================================
	LCD_Command(LCD_SETDDRAMADDR | 7);
	LCD_Char(0x31);
//=============================================================================================================
*/

/*
//============== Пример: вывести "@?>=<;:9876543210" в первой строке ==========================================
	for (uint8_t i = 0; i<16; i++)
	{
		LCD_Command(LCD_SETDDRAMADDR | i);	// в ячейку памяти i
		LCD_Char(0x30+16-i);
	}
//=============================================================================================================
*/

/*
//============== Пример: вывести "0123456789:;>=<?@" во второй строке =========================================
	for (uint8_t i = 0; i<16; i++)
	{
		LCD_Command(LCD_SETDDRAMADDR | SECONDSTRING+i);	// в ячейка памяти в первом ряду в позицию 8
		LCD_Char(0x30+i);
	}
//=============================================================================================================
*/
}



void LCD_WriteData(uint8_t data)
{
	if(((data >> 3)&0x01)==1) 	{d7_set;} 		else 	{d7_reset;}
	if(((data >> 2)&0x01)==1) 	{d6_set;} 		else 	{d6_reset;}
	if(((data >> 1)&0x01)==1) 	{d5_set;} 		else 	{d5_reset;}
	if((data&0x01)==1) 			{d4_set;} 		else 	{d4_reset;}
}



void LCD_Command(uint8_t cmd)
{
	rs0;					//	говорим, что сейчас оправляем команду в экран
	LCD_Send(cmd);			//	пишем
	rs1;					//	говорим, что сейчас оправляем символ в экран	(просто для удобства)
}



void LCD_Char(char ch)
{
	rs1;					//	говорим, что сейчас оправляем символ в экран
	LCD_Send(ch);			//	пишем
}


void LCD_Send(char data)
{
//============== Отправляем первую половину байта =============================================================
							e1;
							//delay_us(1);	// на моем дисплее без этих задержек не работает
	LCD_WriteData(data>>4);
							//delay_us(1);
							e0;
							//delay_us(1);
//=============================================================================================================


//============== Отправляем вторую половну байта ==============================================================
							e1;
							//delay_us(1);
	LCD_WriteData(data);
							//delay_us(1);
							e0;
							//delay_us(1);
//=============================================================================================================


//============== восстанавливаем линию ENABLED ================================================================
							e1;
							//delay_us(1);
//=============================================================================================================


	delay_us(50);	LCD_WriteData(0xFF);	//	восстанавливаем линии данных
}


void LCDdefinechar(const uint8_t *pc,uint8_t char_code){
	uint8_t a, pcc;
	uint16_t i;
	a=(char_code<<3)|0x40;
	for (i=0; i<8; i++){
		pcc=pgm_read_byte(&pc[i]);
		LCDsendCommand(a++);
		LCDsendChar(pcc);
		}
}


void LCDsendString(char *s)
{
	while( *s )
	{
		LCD_Char(*s++);					//		LCD_Char( lcd_rus(*s++) );	//	дисплей должен поддерживать кириллицу
	}
}


static uint8_t lcd_rus(uint8_t c)	//	функцию надо проверять
{
	if  (c > 191)
	{
		c = c - 192-1;
		c= &convert_HD44780[c];
	}
	return c;
}

