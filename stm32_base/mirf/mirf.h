#ifndef _MIRF_H_
#define _MIRF_H_

#include <stdio.h>
#include "nRF24L01.h"


#define	MIRF_Master		//	это устройство - мастер. Если подчиненный, то закоментировать эту строку
#define mirf_PAYLOAD 16


#define NRF_CS_pin		1	//	B10	(PB1)
#define NRF_CE_pin		2	//	B0	(PB2)

#define LOW 	0
#define HIGH 	1

#define RF24_1MBPS		0	//(0 << RF_DR_HIGH)
#define RF24_2MBPS		1	//(1 << RF_DR_HIGH)
#define RF24_250KBPS	2	//(0 << RF_DR_LOW)


void setPALevel(uint8_t level);
#define min_PA 		0
#define low_PA 		1
#define high_PA 	2
#define max_PA 		3

void NRF_CE (uint8_t value);
void NRF_CSN (uint8_t value);
void NRF_cmd(uint8_t cmd);

void NRF_write_buf(uint8_t cmd, uint8_t *buf, uint8_t cnt);	//	write many registers
void NRF_read_buf(uint8_t reg, uint8_t *buf, uint8_t cnt);	//	read many registers

void NRF_write_reg (uint8_t adr, uint8_t data);				//	write one register
uint8_t NRF_read_reg (uint8_t adr);							//	read one register

void NRF_Init(void);										//	initialize mirf

void NRF_int_vect (void);

void GPIO_NRF_Init (void);									//	config GPIO for CE and CSN

void NRF_write (uint8_t *data);

#ifndef uint16_t
#define uint8_t unsigned char
#define uint16_t unsigned int
#endif

void for_test(void);

#endif /* _MIRF_H_ */

