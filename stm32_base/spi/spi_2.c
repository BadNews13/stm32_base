/*
 * spi_2.c
 *
 *  Created on: Jun 25, 2021
 *      Author: bad_n
 */



#include <spi_2.h>


void SPI2_Init(void)
{
	SPI2_GPIO_Init();

  SET_BIT(RCC->APB1ENR, RCC_APB1ENR_SPI2EN);		 			 //RCC peripheral clock enabling
  uint8_t tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_SPI2EN);	 //Delay after an RCC peripheral clock enabling

uint16_t CR1_reg = 0;

CLEAR_BIT	(CR1_reg, SPI_CR1_BIDIMODE);		//	Bit 15 BIDIMODE: Bidirectional data mode enable
CLEAR_BIT	(CR1_reg, SPI_CR1_BIDIOE);			//	Bit 14 BIDIOE: Output enable in bidirectional mode
CLEAR_BIT	(CR1_reg, SPI_CR1_CRCEN);			//	Bit 13 CRCEN: Hardware CRC calculation enable
CLEAR_BIT	(CR1_reg, SPI_CR1_CRCNEXT);			//	Bit 12 CRCNEXT: CRC transfer next
CLEAR_BIT	(CR1_reg, SPI_CR1_DFF);				//	Bit 11 DFF: Data frame format
CLEAR_BIT	(CR1_reg, SPI_CR1_RXONLY);			//	Bit 10 RXONLY: Receive only
SET_BIT		(CR1_reg, SPI_CR1_SSM);				//	Bit 9 SSM: Software slave management
SET_BIT		(CR1_reg, SPI_CR1_SSI);				//	Bit 8 SSI: Internal slave select
CLEAR_BIT	(CR1_reg, SPI_CR1_LSBFIRST);		//	Bit 7 LSBFIRST: Frame format
CLEAR_BIT	(CR1_reg, SPI_CR1_SPE);				//	Bit 6 SPE: SPI enable

SET_BIT		(CR1_reg, SPI_CR1_BR_2);
SET_BIT 	(CR1_reg, SPI_CR1_BR_1);			//	//	Bits 5:3 BR[2:0]: Baud rate control
SET_BIT 	(CR1_reg, SPI_CR1_BR_0);

SET_BIT 	(CR1_reg, SPI_CR1_MSTR);			//	Bit 2 MSTR: Master selection
CLEAR_BIT	(CR1_reg, SPI_CR1_CPOL);			//	Bit 1 CPOL: Clock polarity
CLEAR_BIT	(CR1_reg, SPI_CR1_CPHA);			//	Bit 0 CPHA: Clock phase


WRITE_REG (SPI2->CR1,CR1_reg);					//	Bit 6 SPE: SPI enable

/*
  //8 bit 0:0
  MODIFY_REG(SPI2->CR1, SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE | SPI_CR1_CRCEN | SPI_CR1_CRCNEXT | \
                        SPI_CR1_DFF | SPI_CR1_RXONLY | SPI_CR1_LSBFIRST | \
                        SPI_CR1_BR_2 | SPI_CR1_CPOL | SPI_CR1_CPHA , \
                        SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_BR_1 | SPI_CR1_BR_0 | SPI_CR1_MSTR);
*/

  SET_BIT(SPI2->CR1, SPI_CR1_SPE);		//	Bit 6 SPE: SPI enable

  CLEAR_BIT(SPI2->CR2, 0x00FF);
}


void SPI2_GPIO_Init (void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

	uint8_t offset;

	// PB13 - SCK2
	offset = (pin_SPI2_SCK - 8) * 4;							//	5 * 4 = 20
	GPIOB->CRH &= ~( GPIO_BITS_MASK << offset );		//	стереть 4 бита
	GPIOB->CRH |= ( AF_PUSH_PULL << offset );			//	записать 4 бита
	//GPIOA->BSRR = ( 1 << pin_SPI1_SCK );				//	установка линии в 1
	//GPIOA->BRR = ( 1 << pin_SPI1_SCK );				//	установка линии в 0

	// PB14 - MISO2
	offset = (pin_SPI2_MISO - 8) * 4;							//	6 * 4 = 24
	GPIOB->CRH &= ~( GPIO_BITS_MASK << offset );		//	стереть 4 бита
	GPIOB->CRH |= ( INPUT_FLOATING << offset );			//	записать 4 бита
//	GPIOB->BSRR = ( 1 << pin_SPI2_MISO );				//	установка линии в 1
	//GPIOA->BRR = ( 1 << pin_SPI1_MISO );				//	установка линии в 0

	// PB15 - MOSI2
	offset = (pin_SPI2_MOSI - 8) * 4;							//	7 * 4 = 28
	GPIOB->CRH &= ~( GPIO_BITS_MASK << offset );		//	стереть 4 бита
	GPIOB->CRH |= ( AF_PUSH_PULL << offset );			//	записать 4 бита
	GPIOB->BSRR = ( 1 << pin_SPI2_MOSI );				//	установка линии в 1
	//GPIOA->BRR = ( 1 << pin_SPI1_MOSI );				//	установка линии в 0


}

uint8_t SPI2_put_byte (uint8_t tx_byte)
{
	SPI2->DR = tx_byte;
	while(READ_BIT(SPI2->SR, SPI_SR_BSY));
	uint8_t rx_byte = SPI2->DR;
	return rx_byte;
}







