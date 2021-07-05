
#ifndef delay_ms_H_
#define delay_ms_H_

#include "stm32f10x.h"

void SysTick_Init(void);

//extern "C" void SysTick_Handler(void);	//	обработчик прерывания системного счетчика
void SysTick_Handler(void);

void delay_ms(uint32_t ms);


#endif /* delay_ms_H_ */




