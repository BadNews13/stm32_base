/*
 * time_out.h
 *
 *  Created on: 5 июл. 2021 г.
 *      Author: bad_n
 */

#ifndef TIME_OUT_H_
#define TIME_OUT_H_



#include "stm32f10x.h"

void timerBLINK_ini (void);
void TIM3_IRQHandler(void);


void timerBLINK_ini (void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;		//Тактирование таймера TIM3

//	основная шина 72 000 000 Hz
//	шина APB1 = 72 000 000 Hz / 2 = 36 000 000

// если мы считаем микросекунды в прерывании, то точность падает.
	// прерывание 1 раз в секунду
	TIM3->PSC = 36000-1;			//	36 000 000 sis_tick / 36 000 = 1 000 Hz (1000 us)	//	Настройка предделителя таймера
	TIM3->ARR = 2000-1;				//	1 Interrupt / 2 0000 timer_tick 	//	Загружаем число миллисекунд в регистр автоперезагрузки

	TIM3->DIER |= TIM_DIER_UIE; 	//	Enable tim2 interrupt		//	Разрешаем прерывание при переполнении счетчика
	TIM3->CR1 |= TIM_CR1_CEN;   	//	Start count					//	Запускаем счет

	NVIC_EnableIRQ(TIM3_IRQn);  	//	Enable IRQ
}


void TIM3_IRQHandler(void)
{
	static uint16_t s  = 0;			//	секунды
	s++;

	if (s == 1)
	{
		static uint8_t i=0;

		if (1 == (i++ & 0x1))		{GPIOC->BSRR = GPIO_BSRR_BS13;}		//	установить нулевой бит		(выключить светодиод)
		else						{GPIOC->BRR = ( 1 << 13 );}			//	сбросить нулевой бит		(включить светодиод)

		s = 0;
	}

	TIM3->SR &= ~TIM_SR_UIF;		//	Clean UIF Flag
}



#endif /* TIME_OUT_H_ */
