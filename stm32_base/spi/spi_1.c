/*
 * spi_1.c
 *
 *  Created on: Jun 10, 2021
 *      Author: bad_n
 */
#include <spi_1.h>




void SPI1_Init (void)
{
	SPI1_GPIO_Init();

	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN);	  										//RCC peripheral clock enabling
	uint8_t tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_SPI2EN); 	(void) tmpreg;	  //Delay after an RCC peripheral clock enabling

	WRITE_REG	(SPI1->CR1,	SPI_CR1_BR & ( SPI_CR1_BR_2 |  SPI_CR1_BR_1 |  SPI_CR1_BR_0));	//	111	/256	BaudRate (самый медленный)

	CLEAR_BIT 	(SPI1->CR1, SPI_CR1_CPHA);			//	Bit 0 CPHA:			Clock phase
	CLEAR_BIT 	(SPI1->CR1, SPI_CR1_CPOL);			//	Bit 1 CPOL:			Clock polarity							0: CK to 0 when idle
	SET_BIT 	(SPI1->CR1, SPI_CR1_MSTR);			//	Bit 2 MSTR:			Master selection						1: Master configuration

	CLEAR_BIT	(SPI1->CR1, SPI_CR1_SPE);			//	Bit 6 SPE:			SPI enable								0: Peripheral disabled
	CLEAR_BIT	(SPI1->CR1, SPI_CR1_LSBFIRST);		//	Bit 7 LSBFIRST:		Frame format							0: MSB transmitted first
	SET_BIT		(SPI1->CR1, SPI_CR1_SSI);			//	Bit 8 SSI:			Internal slave select
	SET_BIT		(SPI1->CR1, SPI_CR1_SSM);			//	Bit 9 SSM:			Software slave management
	CLEAR_BIT	(SPI1->CR1, SPI_CR1_RXONLY);		//	Bit 10 RXONLY:		Receive only							0: Full duplex (Transmit and receive)
	CLEAR_BIT	(SPI1->CR1, SPI_CR1_DFF);			//	Bit 11 DFF:			Data frame format						0: 8-bit data frame format is selected for transmission/reception
	CLEAR_BIT	(SPI1->CR1, SPI_CR1_CRCNEXT);		//	Bit 12 CRCNEXT:		CRC transfer next						0: Data phase (no CRC phase)
	CLEAR_BIT	(SPI1->CR1, SPI_CR1_CRCEN);			//	Bit 13 CRCEN:		Hardware CRC calculation enable			0: CRC calculation disabled
	CLEAR_BIT	(SPI1->CR1, SPI_CR1_BIDIOE);		//	Bit 14 BIDIOE:		Output enable in bidirectional mode		0: Output disabled (receive-only mode) in bidirectional mode
	CLEAR_BIT	(SPI1->CR1, SPI_CR1_BIDIMODE);		//	Bit 15 BIDIMODE:	Bidirectional data mode enable			0: 2-line unidirectional data mode selected

//	uint8_t rx_byte = SPI1->DR;						//	чтобы с флагами разобраться
//	rx_byte = READ_REG(SPI1->SR);

	SET_BIT		(SPI1->CR1, SPI_CR1_SPE);			//	SPI enable
	//WRITE_REG(SPI1->DR, 0xA2);					//	отправить байт в SPI
}




void SPI1_GPIO_Init (void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	uint8_t offset;


	// PA5 - SCK1
	offset = pin_SPI1_SCK * 4;							//	5 * 4 = 20
	GPIOA->CRL &= ~( GPIO_BITS_MASK << offset );		//	стереть 4 бита
	GPIOA->CRL |= ( AF_PUSH_PULL << offset );			//	записать 4 бита
	//GPIOA->BSRR = ( 1 << pin_SPI1_SCK );				//	установка линии в 1
	//GPIOA->BRR = ( 1 << pin_SPI1_SCK );				//	установка линии в 0

	// PA6 - MISO1
	offset = pin_SPI1_MISO * 4;							//	6 * 4 = 24
	GPIOA->CRL &= ~( GPIO_BITS_MASK << offset );		//	стереть 4 бита
	GPIOA->CRL |= ( INPUT_FLOATING << offset );			//	записать 4 бита
	GPIOA->BSRR = ( 1 << pin_SPI1_MISO );				//	установка линии в 1
	//GPIOA->BRR = ( 1 << pin_SPI1_MISO );				//	установка линии в 0

	// PA7 - MOSI1
	offset = pin_SPI1_MOSI * 4;							//	7 * 4 = 28
	GPIOA->CRL &= ~( GPIO_BITS_MASK << offset );		//	стереть 4 бита
	GPIOA->CRL |= ( AF_PUSH_PULL << offset );			//	записать 4 бита
	GPIOA->BSRR = ( 1 << pin_SPI1_MOSI );				//	установка линии в 1
	//GPIOA->BRR = ( 1 << pin_SPI1_MOSI );				//	установка линии в 0


}

uint8_t SPI1_put_byte (uint8_t tx_byte)
{
	SPI1->DR = tx_byte;
	while(READ_BIT(SPI1->SR, SPI_SR_BSY));
	uint8_t rx_byte = SPI1->DR;
	return rx_byte;
}










