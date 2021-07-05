

#include <delay_ms.h>

static __IO uint32_t SysTick_CNT = 0;	//	для системного таймера
#define SYSCLOCK 72000000U

void SysTick_Init(void)
{
	  MODIFY_REG(SysTick->LOAD,SysTick_LOAD_RELOAD_Msk,SYSCLOCK / 1000 - 1);
	  CLEAR_BIT(SysTick->VAL, SysTick_VAL_CURRENT_Msk);
	  SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
}

//extern "C" void SysTick_Handler(void)	//	обработчик прерывания системного счетчика
void SysTick_Handler(void)	//	обработчик прерывания системного счетчика
{
	if(SysTick_CNT > 0)  SysTick_CNT--;
}

void delay_ms(uint32_t ms)
{
	MODIFY_REG(SysTick->VAL,SysTick_VAL_CURRENT_Msk,SYSCLOCK / 1000 - 1);
	SysTick_CNT = ms;
	while(SysTick_CNT) {}
}

