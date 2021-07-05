

#include <delay_us.h>

static __IO uint32_t SysTick_CNT_us = 0;	//	для системного таймера

void timer_delay_us_ini (void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;		//Тактирование таймера TIM4

//	основная шина 72 000 000 Hz
//	шина APB1 = 72 000 000 Hz / 2 = 36 000 000

// если мы считаем микросекунды в прерывании, то точность падает.
	// прерывание 1 раз в микросекунду
	TIM4->PSC = 36-1;			//	36 000 000 sis_tick / 36 000 = 1 000 Hz (1000 us)	//	Настройка предделителя таймера
	TIM4->ARR = 2-1;				//	1 Interrupt / 2 0000 timer_tick 	//	Загружаем число миллисекунд в регистр автоперезагрузки

	TIM4->SR &= ~TIM_SR_UIF;		//	Clean UIF Flag
	TIM4->DIER |= TIM_DIER_UIE; 	//	Enable tim2 interrupt		//	Разрешаем прерывание при переполнении счетчика
//	TIM4->CR1 |= TIM_CR1_CEN;   	//	Start count					//	Запускаем счет

	NVIC_EnableIRQ(TIM4_IRQn);  	//	Enable IRQ
}


void TIM4_IRQHandler(void)
{
	if(SysTick_CNT_us > 0)  SysTick_CNT_us--;
	TIM4->SR &= ~TIM_SR_UIF;		//	Clean UIF Flag
}





void delay_us(uint32_t us)
{
	TIM4->CR1 |= TIM_CR1_CEN;   	//	Start count					//	Запускаем счет
	SysTick_CNT_us = us;
	while(SysTick_CNT_us) {}
	TIM4->CR1 &= ~TIM_CR1_CEN;   	//	Stop count					//	останавливаем счет
}

