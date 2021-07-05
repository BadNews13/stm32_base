
#ifndef delay_us_H_
#define delay_us_H_

#include "stm32f10x.h"

void timer_delay_us_ini (void);
void TIM4_IRQHandler(void);
void delay_us(uint32_t us);

#endif /* delay_us_H_ */
