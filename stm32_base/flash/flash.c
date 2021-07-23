

#include <flash.h>


void FLASH_Init(void) {
	/* Next commands may be used in SysClock initialization function
	   In this case using of FLASH_Init is not obligatorily */
	/* Enable Prefetch Buffer */
	FLASH_PrefetchBufferCmd( FLASH_PrefetchBuffer_Enable);
	/* Flash 2 wait state */
	FLASH_SetLatency( FLASH_Latency_2);




	/*
			uint32_t word;
			FLASH_Unlock();

			FLASH_Erase_Page(((uint32_t)0x08007C00));	//	стираем все страницу (указывая начало)

			word = 0x00000000;
			write_word((uint32_t)(0x08007C00), word);	//	пишем слово в начало страницы

			word = 0x01020304;
			write_word((uint32_t)(0x08007C04), word);	//	пишем слово в пишем в следующие 32 бита (4 байта)

			word = 0x05060708;
			write_word((uint32_t)(0x08007C08), word);	//	пишем слово в пишем в следующие 32 бита (4 байта)

			FLASH_Lock();
	*/


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
	if (FLASH->SR & FLASH_SR_EOP) {FLASH->SR = FLASH_SR_EOP;}

	FLASH->CR|= FLASH_CR_PER; 			//	Устанавливаем бит стирания одной страницы
	FLASH->AR = address; 				//	Задаем её адрес
	FLASH->CR|= FLASH_CR_STRT; 			//	Запускаем стирание

	while (FLASH->SR & FLASH_SR_BSY);	//	Ждем пока страница сотрется

	FLASH->CR &= ~FLASH_CR_PER;	 	//	Сбрасываем бит стирания одной страницы

	if(FLASH->SR & FLASH_SR_EOP)	{FLASH->SR |= FLASH_SR_EOP;	return 1;}	//	операция завершена, очищаем флаг
	else							{return 1;}
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

	for (i = 0; i < count; i += 2)
	{
		*(volatile unsigned short*)(address + i) = (((unsigned short)data[i + 1]) << 8) + data[i];

		while (!(FLASH->SR & FLASH_SR_EOP));
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR &= ~(FLASH_CR_PG);
}

uint32_t read_word(uint32_t address)
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


void write_word (uint32_t Address, uint32_t *Data, uint16_t cnt_of_bytes)
{
	uint16_t cnt_of_word = cnt_of_bytes / 4;	//	четыре байта в одном слове. Не кратный хвост не обрабатывается (т.к. деление на цело)
	uint32_t Address_for_half_word;

	for (uint8_t i = 0; i < cnt_of_word; i++)
	{
		while (FLASH->SR & FLASH_SR_BSY);
		if (FLASH->SR & FLASH_SR_EOP) {FLASH->SR = FLASH_SR_EOP;}	//	сбрасываем флаг завершения операции

	    /* if the previous operation is completed, proceed to program the new first
	    half word */
	    FLASH->CR |= CR_PG_Set;		//	устанавливаем флаг - программирование flash

//======First half-word======================================================================================
	    Address_for_half_word = Address + (0x4 *i);
	    *(__IO uint16_t*) Address_for_half_word = (uint16_t)Data[i];

		while (!(FLASH->SR & FLASH_SR_EOP));
		FLASH->SR = FLASH_SR_EOP;
//===========================================================================================================

//======Second half-word=====================================================================================
		Address_for_half_word = Address + (0x4 *i) + 2;
	    *(__IO uint16_t*) Address_for_half_word = (uint16_t)(Data[i]>>16);

		while (!(FLASH->SR & FLASH_SR_EOP));	//	ждем завершения операции
		FLASH->SR = FLASH_SR_EOP;				//	сбрасываем флаг завершения операции
//===========================================================================================================
	}

   return;
}

void write_Page (uint32_t Address, uint32_t *Data )
{
//	uint16_t cnt_of_word = Page_size / 4;	//	четыре байта в одном слове. Не кратный хвост не обрабатывается (т.к. деление на цело)

//	uint8_t cnt_of_word = Page_size / 32; 	// столько 32-х битных слов в 1024 байтной странице
	uint8_t cnt_of_word = 0xFF;

	uint32_t Address_for_half_word;

	for (uint8_t i = 0; i < cnt_of_word; i++)
	{
		while (FLASH->SR & FLASH_SR_BSY);
		if (FLASH->SR & FLASH_SR_EOP) {FLASH->SR = FLASH_SR_EOP;}	//	сбрасываем флаг завершения операции

	    /* if the previous operation is completed, proceed to program the new first
	    half word */
	    FLASH->CR |= CR_PG_Set;		//	устанавливаем флаг - программирование flash

//======First half-word======================================================================================
	    Address_for_half_word = Address + (0x4 *i);
	    *(__IO uint16_t*) Address_for_half_word = (uint16_t)Data[i];

		while (!(FLASH->SR & FLASH_SR_EOP));
		FLASH->SR = FLASH_SR_EOP;
//===========================================================================================================

//======Second half-word=====================================================================================
		Address_for_half_word = Address + (0x4 *i) + 2;
	    *(__IO uint16_t*) Address_for_half_word = (uint16_t)(Data[i]>>16);

		while (!(FLASH->SR & FLASH_SR_EOP));	//	ждем завершения операции
		FLASH->SR = FLASH_SR_EOP;				//	сбрасываем флаг завершения операции
//===========================================================================================================

		FLASH->CR &= CR_PG_Reset;
	}

   return;
}

