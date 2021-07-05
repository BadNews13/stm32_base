/*
 * lcd.h
 *
 *  Created on: Apr 15, 2021
 *      Author: bad_n
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

//#include "stm32f10x.h"
#include <global_defines.h>


// commands
#define LCD_CLEARDISPLAY 	0x01
#define LCD_RETURNHOME 		0x02
#define LCD_ENTRYMODESET 	0x04
#define LCD_DISPLAYCONTROL 	0x08
#define LCD_CURSORSHIFT 	0x10
#define LCD_FUNCTIONSET 	0x20
#define LCD_SETCGRAMADDR 	0x40
#define LCD_SETDDRAMADDR 	0x80


#define SECONDSTRING 		0x40



//======== Пины дисплея ===========================================================
#define pin_RS	9
#define pin_RW	8
#define pin_E	7

#define pin_D4	6
#define pin_D5	12
#define pin_D6	4
#define pin_D7	3

#define pin_bright	15
//=================================================================================


//======== Макросы для переключения состояний выводов =============================
#define rs1				GPIOB->BSRR = ( 1 << pin_RS )		// установка линии RegisterSelest в 1 (данные)
#define rs0				GPIOB->BRR = ( 1 << pin_RS )		// установка линии RegisterSelest в 0 (команда)

#define rw1   			GPIOB->BSRR = ( 1 << pin_RW ) 		// установка линии Read/Write в 1
#define rw0   			GPIOB->BRR = ( 1 << pin_RW ) 		// установка линии Read/Write в 0

#define e1				GPIOB->BSRR = ( 1 << pin_E ) 		// установка линии Enabled в 1
#define e0    			GPIOB->BRR = ( 1 << pin_E )			// установка линии Enabled в 0


#define d4_set 			GPIOB->BSRR = ( 1 << pin_D4 )		// установка линии Data_4 в 1
#define d4_reset 		GPIOB->BRR = ( 1 << pin_D4 )		// установка линии Data_4 в 0

#define d5_set 			GPIOB->BSRR = ( 1 << pin_D5 )		// установка линии Data_4 в 1
#define d5_reset 		GPIOB->BRR = ( 1 << pin_D5 )		// установка линии Data_5 в 0

#define d6_set 			GPIOB->BSRR = ( 1 << pin_D6 )		// установка линии Data_4 в 1
#define d6_reset 		GPIOB->BRR = ( 1 << pin_D6 )		// установка линии Data_6 в 0

#define d7_set 			GPIOB->BSRR = ( 1 << pin_D7 )		// установка линии Data_4 в 1
#define d7_reset 		GPIOB->BRR = ( 1 << pin_D7 )		// установка линии Data_7 в 0


#define bright_off		GPIOA->BSRR = ( 1 << pin_bright ) 	// установка линии Contrast в 1
#define bright_on		GPIOA->BRR = ( 1 << pin_bright )	// установка линии Contrast в 0

//=================================================================================

void LCD_init(void);

void LCD_Command(uint8_t dt);
void LCD_Char(char ch);

void LCD_Send(char data);
void LCD_WriteData(uint8_t dt);

void LCDsendString(char *s);
static uint8_t lcd_rus(uint8_t c);

#endif /* INC_LCD_H_ */
