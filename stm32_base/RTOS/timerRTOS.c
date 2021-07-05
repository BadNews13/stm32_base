

#include <timerRTOS.h>
#include <rtos.h>

void timerRTOS_ini (void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;		//Тактирование таймера TIM3

//	основная шина 72 000 000 Hz
//	шина APB1 = 72 000 000 Hz / 2 = 36 000 000

// если мы считаем микросекунды в прерывании, то точность падает.
	// прерывание 1 раз в секунду
	TIM2->PSC = 36000-1;			//	36 000 000 sis_tick / 36 000 = 1 000 Hz (1000 us)	//	Настройка предделителя таймера
	TIM2->ARR = 2-1;				//	1 Interrupt / 2 0000 timer_tick 	//	Загружаем число миллисекунд в регистр автоперезагрузки

	TIM2->DIER |= TIM_DIER_UIE; 	//	Enable tim2 interrupt		//	Разрешаем прерывание при переполнении счетчика
	TIM2->CR1 |= TIM_CR1_CEN;   	//	Start count					//	Запускаем счет

	NVIC_EnableIRQ(TIM2_IRQn);  	//	Enable IRQ
}


 void TIM2_IRQHandler(void)
{
//	 put_byte_UART2(0xAA);
	RTOS_ISR_INT();					//	обработчик RTOS
	TIM2->SR &= ~TIM_SR_UIF;		//	Clean UIF Flag
}
