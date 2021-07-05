
#include <uart_1.h>




void uart1_init (uint32_t BaudRate)
{
	uint32_t F_CPU = 72000000;

	uart1_port_ini();

	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);	//	тактирование периферии UART1

	////USART needs to be in disabled state, in order to be able to configure some bits in CRx registers
	if(READ_BIT(USART1->CR1, USART_CR1_UE) != (USART_CR1_UE))
	{
		MODIFY_REG(USART1->CR1,
								USART_CR1_M | USART_CR1_PCE | USART_CR1_PS,
								USART_CR1_TE |USART_CR1_RE);
	}

	//	разрешим от данного модуля локальные прерывания – по заполнению приёмного буфера и по ошибке передачи данных
	SET_BIT		(USART1->CR1, USART_CR1_RXNEIE);
	SET_BIT		(USART1->CR3, USART_CR3_EIE);

	//Async Mode
	CLEAR_BIT	(USART1->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
	CLEAR_BIT	(USART1->CR3, (USART_CR3_SCEN | USART_CR3_IREN | USART_CR3_HDSEL));

	WRITE_REG	(USART1->BRR, F_CPU/BaudRate);										//BaudRate
	SET_BIT		(USART1->CR1, USART_CR1_UE);										//Enable

	NVIC_EnableIRQ(USART1_IRQn);
	USART1_DMA_init();
//	USART1->DR = 0xA1;		//	для теста
}


void uart1_port_ini(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;				//	тактирование порта A

	uint8_t offset; 								//	смещение в регистре

//============== Настройка вывода TX (PA9) ==================================================================================
	offset = ( USART1_TX_pin - 8 ) * 4;				//	(9-8) * 4 = 4
	GPIOA->CRH &= ~( GPIO_BITS_MASK << offset );	//
	GPIOA->CRH |= ( AF_PUSH_PULL << offset );		//	альтернативная функция с выходом пуш-пул	AF_PUSH_PULL
//===========================================================================================================================

//============== Настройка вывода RX (PA10) =================================================================================
	offset = ( USART1_RX_pin - 8 ) * 4;				//	(10-8) * 4 = 8
	GPIOA->CRH &= ~( GPIO_BITS_MASK << offset );	//
	GPIOA->CRH |= ( INPUT_FLOATING << offset );		//	вход без подтяжки 							INPUT_FLOATING
//===========================================================================================================================
}


void USART1_IRQHandler(void)
{
//	GPIOB->BRR = ( 1 << 10 );							//	установка линии TX3 в 1 (диод не светится)
	if(		(READ_BIT(USART1->SR, USART_SR_RXNE) 		== 	(USART_SR_RXNE)) &&			//	Read data register not empty
			(READ_BIT(USART1->CR1, USART_CR1_RXNEIE)	== 	(USART_CR1_RXNEIE))		)	//	RXNE interrupt enable
	{
		uint8_t byte =  USART1->DR;		//	надо считать или uart1 зависнет
		(void)byte;
	}
}



void uart1_dmaRX_init(void)
{
	//RX
	CLEAR_BIT	(DMA1_Channel5->CCR, DMA_CCR5_DIR | DMA_CCR5_MEM2MEM);		//	Set transfer direction (Peripheral to Memory)
	CLEAR_BIT	(DMA1_Channel5->CCR, DMA_CCR5_PL); 							//	Set priority level
	SET_BIT		(DMA1_Channel5->CCR, DMA_CCR5_CIRC);						//	Transfer mode: circular
	CLEAR_BIT	(DMA1_Channel5->CCR, DMA_CCR5_PINC);						//	Set peripheral no increment mode
	SET_BIT		(DMA1_Channel5->CCR, DMA_CCR5_MINC);						//	Set memory increment mode
	CLEAR_BIT	(DMA1_Channel5->CCR, DMA_CCR5_PSIZE_1 | DMA_CCR5_PSIZE_0);	//	Set peripheral data width
	CLEAR_BIT	(DMA1_Channel5->CCR, DMA_CCR5_MSIZE_1 | DMA_CCR5_MSIZE_0);	//	Set memory data width

	WRITE_REG	(DMA1_Channel5->CPAR, (uint32_t)&(USART1->DR));
	WRITE_REG	(DMA1_Channel5->CMAR, (uint32_t)&uart1_rx_buf);

	CLEAR_BIT	(DMA1_Channel5->CCR, DMA_CCR5_EN);
	WRITE_REG	(DMA1->IFCR, DMA_IFCR_CGIF5);				//	Clear Channel 5 global interrupt flag
	WRITE_REG	(DMA1->IFCR, DMA_IFCR_CTCIF5);				//	Clear Channel 5  transfer complete flag
	WRITE_REG	(DMA1->IFCR, DMA_IFCR_CTEIF5);				//	Clear Channel 5 transfer error flag

	SET_BIT		(USART1->CR3, USART_CR3_DMAR);				//	Enable DMA Mode for reception

	SET_BIT		(DMA1_Channel5->CCR, DMA_CCR5_TCIE);		//	Enable Channel 5 Transfer complete interrupt
	SET_BIT		(DMA1_Channel5->CCR, DMA_CCR5_TEIE);		//	Enable Channel 5 Transfer error interrupt


	MODIFY_REG	(DMA1_Channel5->CNDTR, DMA_CNDTR5_NDT, uart1_rx_buf_size);	//	Set Number of data to transfer
	SET_BIT		(DMA1_Channel5->CCR, DMA_CCR5_EN);			//	Enable DMA channel 5	//	включаем rx канал

	NVIC_EnableIRQ(DMA1_Channel5_IRQn);						//	DMA1_Channel5_IRQn interrupt init
}



void uart1_dmaTX_init(void)
{
	//TX
	MODIFY_REG	(DMA1_Channel4->CCR, DMA_CCR4_MEM2MEM, DMA_CCR4_DIR);		//	Set transfer direction (Memory to Peripheral)
	CLEAR_BIT	(DMA1_Channel4->CCR, DMA_CCR4_PL);							//	Set priority level
	CLEAR_BIT	(DMA1_Channel4->CCR, DMA_CCR4_CIRC);						//	Transfer mode NORMAL
	CLEAR_BIT	(DMA1_Channel4->CCR, DMA_CCR4_PINC);						//	Set peripheral no increment mode
	SET_BIT		(DMA1_Channel4->CCR, DMA_CCR4_MINC);		  				//	Set memory increment mode
	CLEAR_BIT	(DMA1_Channel4->CCR, DMA_CCR4_PSIZE_1 | DMA_CCR4_PSIZE_0);	//	Set peripheral data width
	CLEAR_BIT	(DMA1_Channel4->CCR, DMA_CCR4_MSIZE_1 | DMA_CCR4_MSIZE_0);	//	Set memory data width

	WRITE_REG	(DMA1_Channel4->CPAR, (uint32_t)&(USART1->DR));
	WRITE_REG	(DMA1_Channel4->CMAR, (uint32_t)&uart1_tx_buf);

	CLEAR_BIT	(DMA1_Channel4->CCR, DMA_CCR4_EN);
	WRITE_REG	(DMA1->IFCR, DMA_IFCR_CGIF4);			//	Clear Channel 4 global interrupt flag
	WRITE_REG	(DMA1->IFCR, DMA_IFCR_CTCIF4);			//	Clear Channel 4  transfer complete flag
	WRITE_REG	(DMA1->IFCR, DMA_IFCR_CTEIF4);			//	Clear Channel 4 transfer error flag

	SET_BIT		(USART1->CR3, USART_CR3_DMAT);			//	Enable DMA Mode for transmission

	SET_BIT		(DMA1_Channel4->CCR, DMA_CCR4_TCIE);	//	Enable Channel 4 Transfer complete interrupt
	SET_BIT		(DMA1_Channel4->CCR, DMA_CCR4_TEIE);	//	Enable Channel 4 Transfer error interrupt

	NVIC_EnableIRQ(DMA1_Channel4_IRQn);					//DMA1_Channel4_IRQn interrupt init

}


void DMA1_Channel5_IRQHandler(void)	//	закончился прием от ПК (RX)
{
//	GPIOA->BRR = ( 1 << 2 );		// установка линии в 0 (диод светится)	//	включиться только после пересылки настроенного количества байт
	if(READ_BIT(DMA1->ISR, DMA_ISR_TCIF5) == (DMA_ISR_TCIF5)) // если поднят флаг - завершена пересылка
	{
		WRITE_REG(DMA1->IFCR, DMA_IFCR_CTCIF5);					//	сбрасывем флаг записываю в него 1
	}
	else if(READ_BIT(DMA1->ISR, DMA_ISR_TEIF5) == (DMA_ISR_TEIF5))	//	если поднят бит - ошибка передачи
	{
//		CLEAR_BIT(DMA1_Channel4->CCR, DMA_CCR4_EN);		//Disable DMA channels 4
//		CLEAR_BIT(DMA1_Channel5->CCR, DMA_CCR5_EN);		//Disable DMA channels 5
	}
}


void DMA1_Channel4_IRQHandler(void)	//	закончилась отправка (TX)
{
	if(READ_BIT(DMA1->ISR, DMA_ISR_TCIF4) == (DMA_ISR_TCIF4))		//	если передали данные из памяти в периферию
	{
		// узнаем докинули ли нам еще байт пока отправляли
		// или же мы просто обрабатываем переход кольцевого буфера к началу
		if (uart1_tx_counter)
		{
			if (uart1_tx_write_index >= uart1_tx_counter)	//	перехода не было // пакет лежит в между началом и концом буфера
			{
				uart1_DMA_TX_start_position = 	uart1_tx_write_index - uart1_tx_counter;	//	считаем стартовую позицию
				uart1_DMA_TX_count = 			uart1_tx_counter;							//	считаем сколько байт надо перенаправить сейчас
				uart1_tx_counter = 				0;											//	считаем остаток
			}

			if (uart1_tx_write_index < uart1_tx_counter)
			{
				if(uart1_tx_counter == 1)				//	переход на одном байте
				{
					uart1_DMA_TX_start_position = 	uart1_tx_buf_size - 1;	//	считаем стартовую позицию
					uart1_DMA_TX_count = 			1;						//	считаем сколько байт надо перенаправить сейчас
					uart1_tx_counter = 				0;						//	считаем остаток
				}
				else							//	переход на пакете
				{
					uart1_DMA_TX_start_position = 	(uart1_tx_buf_size - (uart1_tx_counter - uart1_tx_write_index));	//	считаем стартовую позицию	//	узнаем индекс начала пакета в буфере
					uart1_DMA_TX_count = 			uart1_tx_counter - uart1_tx_write_index;							//	только до конца буфера		// узнаем сколько байт лежит до конца буфера
					uart1_tx_counter = 				uart1_tx_counter - uart1_DMA_TX_count;								//	срезаем с счетчика отправлямое количество
				}
			}

			//			DMA_transaction(&tx_buf[DMA_TX_start_position], DMA_TX_count);			//	отправляем

			//	чтобы обойтись без кастыля, то используем код из этого метода
			//	Запускаем перенаправление
			CLEAR_BIT	(DMA1_Channel4->CCR, DMA_CCR4_EN);						//	Disable DMA channel 4
			WRITE_REG	(DMA1_Channel4->CMAR, (uint32_t)&uart1_tx_buf[uart1_DMA_TX_start_position]);	//	указываем с какого места памяти делать транзакцию (в uart)
			//	указываем сколько байт надо перенаправить
			MODIFY_REG	(DMA1_Channel4->CNDTR,										//	Set Number of data to transfer
												DMA_CNDTR4_NDT,						//	сбросить оставшееся количетсво байт для передачи
																uart1_DMA_TX_count);//	записать это значение
			SET_BIT		(DMA1_Channel4->CCR, DMA_CCR4_EN);  						//	Enable DMA channel 4
		}
		else    {CLEAR_BIT(DMA1_Channel4->CCR, DMA_CCR4_EN);}					//	Disable DMA channels 4 // если больше нечего отправлять
	}


	if(READ_BIT(DMA1->ISR, DMA_ISR_TEIF4) == (DMA_ISR_TEIF4))	//	если ошибка передачи
	{
	//Disable DMA channels
		CLEAR_BIT(DMA1_Channel4->CCR, DMA_CCR4_EN);
		CLEAR_BIT(DMA1_Channel5->CCR, DMA_CCR5_EN);
	}

	WRITE_REG(DMA1->IFCR, DMA_IFCR_CTCIF4);				//	сбросим флаг этого прерывания
}





void put_byte_UART1(uint8_t c)
{
//	__disable_irq (); // запретить прерывания		(tx_counter может измениться в прерывании)
	NVIC_DisableIRQ(DMA1_Channel4_IRQn);		//DMA1_Channel4_IRQn interrupt init

	// записываем байт в буфер
	uart1_tx_buf[uart1_tx_write_index++] = c;									//	запишем символ в строку (для DMA доступа)
	if (uart1_tx_write_index == uart1_tx_buf_size)	{uart1_tx_write_index = 0;}	//	если массив закончился, то переходим в начало
	uart1_tx_counter++;															//	увеличим количество байт ожидающих отправку

	//	Если идет передача
	if (READ_REG(DMA_CNDTR4_NDT)&&READ_BIT(DMA1_Channel4->CCR, DMA_CCR4_EN) == (DMA_CCR4_EN))//	если мы еще прошлые байты не отправили и если канал открыт
	{
		//return;	//	в прерывании по завершении перенаправления отработает этот вариант
	}
	else								//	если DMA сейчас не перенаправляет байты
	{
		//	после того как мы положили байт и поняли что линия сейчас "свободна" мы должны передать байты в DMA_транзакцию
		//	если был переход через конец/начало буфера, то отправляем только до конца буфера, а продолжим в прерывании по завершении транзакции

		if(uart1_tx_counter > uart1_tx_write_index)		//	выполнен переход
		{
			if(uart1_tx_counter == 1)				//	переход на одном байте
			{
				uart1_DMA_TX_start_position = 	uart1_tx_buf_size - 1;	//	считаем стартовую позицию
				uart1_DMA_TX_count = 			1;						//	считаем сколько байт надо перенаправить сейчас
				uart1_tx_counter = 				0;						//	считаем остаток
			}
			else							//	переход на пакете
			{
				uart1_DMA_TX_start_position = 	uart1_tx_buf_size - (uart1_tx_counter - uart1_tx_write_index)-1;	//	считаем стартовую позицию	//	узнаем индекс начала пакета в буфере
				uart1_DMA_TX_count = 			uart1_tx_counter - uart1_tx_write_index-1;							//	только до конца буфера		// узнаем сколько байт лежит до конца буфера
				uart1_tx_counter = 				uart1_tx_counter - uart1_DMA_TX_count+1;							//	срезаем с счетчика отправлямое количество
			}
		}
		else								//	перехода не было
		{
			uart1_DMA_TX_start_position = 	uart1_tx_write_index - uart1_tx_counter;	//	считаем стартовую позицию
			uart1_DMA_TX_count = 			uart1_tx_counter;							//	считаем сколько байт надо перенаправить сейчас
			uart1_tx_counter = 				0;											//	считаем остаток
		}
		/*
		for (uint8_t i = 0; i<5; i++)	{}	//	нужна задержка если через вызов метода выполнять запуск DMA, иначе проскакивают двойные байты (кастыль)
		DMA_transaction(&tx_buf[DMA_TX_start_position], DMA_TX_count);
		*/

//	чтобы обойтись без кастыля, то используем код из этого метода
		//	Запускаем перенаправление
		CLEAR_BIT	(DMA1_Channel4->CCR, DMA_CCR4_EN);							//	Disable DMA channel 4
		WRITE_REG	(DMA1_Channel4->CMAR, (uint32_t)&uart1_tx_buf[uart1_DMA_TX_start_position]);	//	указываем с какого места памяти делать транзакцию (в uart)
		//	указываем сколько байт надо перенаправить
		MODIFY_REG	(DMA1_Channel4->CNDTR,										//	Set Number of data to transfer
											DMA_CNDTR4_NDT,						//	сбросить оставшееся количетсво байт для передачи
											  	  	  	  	  uart1_DMA_TX_count);	//	записать это значение
		SET_BIT		(DMA1_Channel4->CCR, DMA_CCR4_EN);  						//	Enable DMA channel 4
	}

//	__enable_irq ();  // разрешить прерывания
	NVIC_EnableIRQ(DMA1_Channel4_IRQn);		//DMA1_Channel4_IRQn interrupt init
}

void USART1_DMA_init(void)
{
	CLEAR_BIT		(USART1->CR1, USART_CR1_UE);								//	Disable uart
	NVIC_DisableIRQ(USART1_IRQn);												//	Запрет на прерывание от uart

	// если DMA работает, то локальные прерывания от UART1 надо запретить
	CLEAR_BIT		(USART1->CR1, USART_CR1_RXNEIE);
	CLEAR_BIT		(USART1->CR3, USART_CR3_EIE);

	SET_BIT			(RCC->AHBENR, RCC_AHBENR_DMA1EN);	//DMA controller clock enable

	uart1_dmaTX_init();	//TX
	uart1_dmaRX_init();	//RX

	SET_BIT		(USART1->CR1, USART_CR1_UE);									//	Enable uart
}


