
#include <exti.h>
#include "mirf.h"

void EXTI_Init (void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; //Тактирование GPIOA
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; //Тактирование AFIO

	uint8_t pin_A8 = 8;
	uint8_t offset = pin_A8 * 4;						//	8 * 4 = 32 (смещение до битов отвечающий за настройку пина)

	GPIOA->CRL &= ~( GPIO_BITS_MASK << offset );		//	стереть 4 бита
	GPIOA->CRL |= ( INPUT_PULL_UP_DOWN << offset );		//	записать 4 бита
//	 GPIOA->CRL |= ( INPUT_FLOATING << offset );		//	записать 4 бита

	GPIOA->BSRR = ( 1 << pin_A8 );						//	установка линии в 1 (диод не светится)
//	GPIOA->BRR = ( 1 << pin_A8 );						//	установка линии TX2 в 0 (диод светится)

	AFIO->EXTICR[0] |= AFIO_EXTICR3_EXTI8_PA;			//	Первый канал EXTI подключен к порту PB1

//	EXTI->RTSR |= EXTI_RTSR_TR1; 						//	Прерывание по нарастанию импульса для канала 1
	EXTI->FTSR |= EXTI_FTSR_TR8;						//	Прерывание по спаду импульса для канала 1

	EXTI->PR = EXTI_PR_PR8;      //Сбрасываем флаг прерывания, перед включением самого прерывания
	EXTI->IMR |= EXTI_IMR_MR8;   //Включаем прерывание 1-го канала EXTI

	NVIC_EnableIRQ(EXTI9_5_IRQn);  //Разрешаем прерывание в контроллере прерываний
}


void EXTI9_5_IRQHandler(void)
{
	put_byte_UART2(0xA8); //delay_ms(10);
	EXTI->PR = EXTI_PR_PR8; //Сбрасываем флаг прерывания

	//	NRF_int_vect();	//	обработка прерывания nrf24l01


	static uint8_t trigger = 0;

	if (trigger)	{GPIOC->BSRR = ( 1 << 13 ); trigger = 0;}		// установка линии в 1
	else			{GPIOC->BRR = ( 1 << 13 ); 	trigger = 1;}		// установка линии в 0
/*
	if (trigger)	{GPIOA->BSRR = ( 1 << 9 ); trigger = 0;}		// установка линии в 1
	else			{GPIOA->BRR = ( 1 << 9 ); 	trigger = 1;}		// установка линии в 0
	*/
}


/*
void EXTI1_IRQHandler(void)
{
	put_byte_UART2(0x55); //delay_ms(10);
	EXTI->PR = EXTI_PR_PR1; //Сбрасываем флаг прерывания


//	NRF_int_vect();	//	обработка прерывания nrf24l01

	static uint8_t trigger = 0;

	if (trigger)	{GPIOC->BSRR = ( 1 << 13 ); trigger = 0;}		// установка линии в 1
	else			{GPIOC->BRR = ( 1 << 13 ); 	trigger = 1;}		// установка линии в 0
}

*/


void EXTI_Init_B1 (void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; //Тактирование GPIOB
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; //Тактирование AFIO

	uint8_t pin_B1 = 1;
	uint8_t offset = pin_B1 * 4;						//	1 * 4 = 4 (смещение до битов отвечающий за настройку пина)

	GPIOB->CRL &= ~( GPIO_BITS_MASK << offset );		//	стереть 4 бита
	GPIOB->CRL |= ( INPUT_PULL_UP_DOWN << offset );		//	записать 4 бита
//	 GPIOB->CRL |= ( INPUT_FLOATING << offset );		//	записать 4 бита

	GPIOB->BSRR = ( 1 << pin_B1 );						//	установка линии в 1 (диод не светится)
//	GPIOB->BRR = ( 1 << pin_B1 );						//	установка линии TX2 в 0 (диод светится)

	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI1_PB;			//	Первый канал EXTI подключен к порту PB1

//	EXTI->RTSR |= EXTI_RTSR_TR1; 						//	Прерывание по нарастанию импульса для канала 1
	EXTI->FTSR |= EXTI_FTSR_TR1;						//	Прерывание по спаду импульса для канала 1

	EXTI->PR = EXTI_PR_PR1;      //Сбрасываем флаг прерывания, перед включением самого прерывания
	EXTI->IMR |= EXTI_IMR_MR1;   //Включаем прерывание 1-го канала EXTI

	NVIC_EnableIRQ(EXTI1_IRQn);  //Разрешаем прерывание в контроллере прерываний
}
