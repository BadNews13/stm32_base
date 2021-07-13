

#include <flash.h>


void FLASH_Init(void) {
	/* Next commands may be used in SysClock initialization function
	   In this case using of FLASH_Init is not obligatorily */
	/* Enable Prefetch Buffer */
	FLASH_PrefetchBufferCmd( FLASH_PrefetchBuffer_Enable);
	/* Flash 2 wait state */
	FLASH_SetLatency( FLASH_Latency_2);
}


void FLASH_Unlock(void)
{
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;
}


void FLASH_Lock(void)
{
	FLASH->CR |= FLASH_CR_LOCK;
}


//address - любой адрес, принадлежащий стираемой странице
_Bool FLASH_Erase_Page(uint32_t address)
{
	while (FLASH->SR & FLASH_SR_BSY);	//	Ожидаем готовности флеша к записи
//	while(!flash_ready());			//	Ожидаем готовности флеша к записи
	if (FLASH->SR & FLASH_SR_EOP) {FLASH->SR = FLASH_SR_EOP;}


	FLASH->CR|= FLASH_CR_PER; 		//	Устанавливаем бит стирания одной страницы
	FLASH->AR = address; 			//	Задаем её адрес
	FLASH->CR|= FLASH_CR_STRT; 		//	Запускаем стирание

//	while(!flash_ready());  		//	Ждем пока страница сотрется
	while (FLASH->SR & FLASH_SR_BSY);	//	Ждем пока страница сотрется

	FLASH->CR &= ~FLASH_CR_PER;	 	//	Сбрасываем бит стирания одной страницы

	return check_EOP();				//	операция завершена, очищаем флаг
}




//data - указатель на записываемые данные
//address - адрес во flash
//count - количество записываемых байт, должно быть кратно 2
//void Internal_Flash_Write(unsigned char* data, unsigned int address, unsigned int count)
void Internal_Flash_Write(unsigned char* data, uint32_t address, unsigned int count)
{
	unsigned int i;

	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP) {FLASH->SR = FLASH_SR_EOP;}

	FLASH->CR |= FLASH_CR_PG;

//	for (uint8_t i = 0; i < count; i++) {}




	for (i = 0; i < count; i += 2)
	{
		*(volatile unsigned short*)(address + i) = (((unsigned short)data[i + 1]) << 8) + data[i];

		while (!(FLASH->SR & FLASH_SR_EOP));
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR &= ~(FLASH_CR_PG);
}

uint32_t flash_read(uint32_t address)
{
	return (*(__IO uint32_t*) address);
}



//_Bool flash_ready(void)		{return !(FLASH->SR & FLASH_SR_BSY);}

_Bool check_EOP(void)
{
	if(FLASH->SR & FLASH_SR_EOP)
	{
		FLASH->SR |= FLASH_SR_EOP;
		return 1;
	}
	return 0;
}





void FLASH_PrefetchBufferCmd(uint32_t FLASH_PrefetchBuffer)
{
  /* Enable or disable the Prefetch Buffer */
  FLASH->ACR &= ACR_PRFTBE_Mask;
  FLASH->ACR |= FLASH_PrefetchBuffer;
}


void FLASH_SetLatency(uint32_t FLASH_Latency)
{
  uint32_t tmpreg = 0;

  /* Read the ACR register */
  tmpreg = FLASH->ACR;

  /* Sets the Latency value */
  tmpreg &= ACR_LATENCY_Mask;
  tmpreg |= FLASH_Latency;

  /* Write the ACR register */
  FLASH->ACR = tmpreg;
}








