/*
 * flash.h
 *
 *  Created on: 12 июл. 2021 г.
 *      Author: bad_n
 */

#ifndef FLASH_FLASH_H_
#define FLASH_FLASH_H_

//#include "stm32f10x.h"
#include <global_defines.h>

//	ключи для разблокировки памяти
#define FLASH_KEY1 ((uint32_t)0x45670123)
#define FLASH_KEY2 ((uint32_t)0xCDEF89AB)

#define Page_size 			0x3FF						//	Размер страницы 1024 byte (0 - 1023)
#define Page_30_ADDR		((uint32_t)0x08007800)		//	Page 30 	0x0800 7800 - 0x0800 7BFF 		1 KB
#define Page_31_ADDR				((uint32_t)0x08007C00)		//	Page 31 	0x0800 7C00 - 0x0800 7FFF 		1 KB

#define size_of_word 		8
#define WORD_CNT 			Page_size/size_of_word
#define WORD_ADDR_OFFSET	0x4


//volatile uint32_t Page[128];							//	массив для работы со страницей Flash памяти


void FLASH_Init(void);
void FLASH_Unlock(void);	//	разблокировать память
void FLASH_Lock(void);		//	заблокировать память
_Bool FLASH_Erase_Page(uint32_t address);
void Internal_Flash_Write(unsigned char* data, uint32_t address, unsigned int count);	//void Internal_Flash_Write(unsigned char* data, unsigned int address, unsigned int count);
uint32_t flash_read(uint32_t address);

_Bool flash_ready(void);	//	Готова ли память к следующей оперции (ждем завершения операции)
_Bool check_EOP(void);
void FLASH_SetLatency(uint32_t FLASH_Latency);
void FLASH_PrefetchBufferCmd(uint32_t FLASH_PrefetchBuffer);



#define FLASH_Latency_0                ((uint32_t)0x00000000)  /*!< FLASH Zero Latency cycle */
#define FLASH_Latency_1                ((uint32_t)0x00000001)  /*!< FLASH One Latency cycle */
#define FLASH_Latency_2                ((uint32_t)0x00000002)  /*!< FLASH Two Latency cycles */
#define IS_FLASH_LATENCY(LATENCY) (((LATENCY) == FLASH_Latency_0) || \
                                   ((LATENCY) == FLASH_Latency_1) || \
                                   ((LATENCY) == FLASH_Latency_2))

#define FLASH_PrefetchBuffer_Enable    ((uint32_t)0x00000010)  /*!< FLASH Prefetch Buffer Enable */
#define FLASH_PrefetchBuffer_Disable   ((uint32_t)0x00000000)  /*!< FLASH Prefetch Buffer Disable */
#define IS_FLASH_PREFETCHBUFFER_STATE(STATE) (((STATE) == FLASH_PrefetchBuffer_Enable) || \
                                              ((STATE) == FLASH_PrefetchBuffer_Disable))




//	для FLASH_PrefetchBufferCmd
#define 	ACR_HLFCYA_Mask   			((uint32_t)0xFFFFFFF7)
#define 	ACR_LATENCY_Mask   			((uint32_t)0x00000038)
#define 	ACR_PRFTBE_Mask  			((uint32_t)0xFFFFFFEF)
#define 	ACR_PRFTBS_Mask   			((uint32_t)0x00000020)
#define 	CR_LOCK_Set   				((uint32_t)0x00000080)
#define 	CR_MER_Reset   				((uint32_t)0x00001FFB)
#define 	CR_MER_Set   				((uint32_t)0x00000004)
#define 	CR_OPTER_Reset   			((uint32_t)0x00001FDF)
#define 	CR_OPTER_Set   				((uint32_t)0x00000020)
#define 	CR_OPTPG_Reset   			((uint32_t)0x00001FEF)
#define 	CR_OPTPG_Set   				((uint32_t)0x00000010)
#define 	CR_PER_Reset   				((uint32_t)0x00001FFD)
#define 	CR_PER_Set   				((uint32_t)0x00000002)
#define 	CR_PG_Reset   				((uint32_t)0x00001FFE)
#define 	CR_PG_Set   				((uint32_t)0x00000001)
#define 	CR_STRT_Set   				((uint32_t)0x00000040)
#define 	EraseTimeout   				((uint32_t)0x000B0000)
#define 	FLASH_BANK1_END_ADDRESS   	((uint32_t)0x807FFFF)
#define 	FLASH_KEY1   				((uint32_t)0x45670123)
#define 	FLASH_KEY2   				((uint32_t)0xCDEF89AB)
#define 	OB_USER_BFB2   				((uint16_t)0x0008)
#define 	ProgramTimeout   			((uint32_t)0x00002000)
#define 	RDP_Key   					((uint16_t)0x00A5)
#define 	RDPRT_Mask   				((uint32_t)0x00000002)
#define 	WRP0_Mask   				((uint32_t)0x000000FF)
#define 	WRP1_Mask   				((uint32_t)0x0000FF00)
#define 	WRP2_Mask   				((uint32_t)0x00FF0000)
#define 	WRP3_Mask   				((uint32_t)0xFF000000)







#ifdef STM32F10X_XL
	#define FLASH_FLAG_BANK2_BSY                 ((uint32_t)0x80000001)  /*!< FLASH BANK2 Busy flag */
	#define FLASH_FLAG_BANK2_EOP                 ((uint32_t)0x80000020)  /*!< FLASH BANK2 End of Operation flag */
	#define FLASH_FLAG_BANK2_PGERR               ((uint32_t)0x80000004)  /*!< FLASH BANK2 Program error flag */
	#define FLASH_FLAG_BANK2_WRPRTERR            ((uint32_t)0x80000010)  /*!< FLASH BANK2 Write protected error flag */

	#define FLASH_FLAG_BANK1_BSY                 FLASH_FLAG_BSY       /*!< FLASH BANK1 Busy flag*/
	#define FLASH_FLAG_BANK1_EOP                 FLASH_FLAG_EOP       /*!< FLASH BANK1 End of Operation flag */
	#define FLASH_FLAG_BANK1_PGERR               FLASH_FLAG_PGERR     /*!< FLASH BANK1 Program error flag */
	#define FLASH_FLAG_BANK1_WRPRTERR            FLASH_FLAG_WRPRTERR  /*!< FLASH BANK1 Write protected error flag */

	#define FLASH_FLAG_BSY                 ((uint32_t)0x00000001)  /*!< FLASH Busy flag */
	#define FLASH_FLAG_EOP                 ((uint32_t)0x00000020)  /*!< FLASH End of Operation flag */
	#define FLASH_FLAG_PGERR               ((uint32_t)0x00000004)  /*!< FLASH Program error flag */
	#define FLASH_FLAG_WRPRTERR            ((uint32_t)0x00000010)  /*!< FLASH Write protected error flag */
	#define FLASH_FLAG_OPTERR              ((uint32_t)0x00000001)  /*!< FLASH Option Byte error flag */

	#define IS_FLASH_CLEAR_FLAG(FLAG) ((((FLAG) & (uint32_t)0x7FFFFFCA) == 0x00000000) && ((FLAG) != 0x00000000))
	#define IS_FLASH_GET_FLAG(FLAG)  (((FLAG) == FLASH_FLAG_BSY) || ((FLAG) == FLASH_FLAG_EOP) || \
									  ((FLAG) == FLASH_FLAG_PGERR) || ((FLAG) == FLASH_FLAG_WRPRTERR) || \
									  ((FLAG) == FLASH_FLAG_OPTERR)|| \
									  ((FLAG) == FLASH_FLAG_BANK1_BSY) || ((FLAG) == FLASH_FLAG_BANK1_EOP) || \
									  ((FLAG) == FLASH_FLAG_BANK1_PGERR) || ((FLAG) == FLASH_FLAG_BANK1_WRPRTERR) || \
									  ((FLAG) == FLASH_FLAG_BANK2_BSY) || ((FLAG) == FLASH_FLAG_BANK2_EOP) || \
									  ((FLAG) == FLASH_FLAG_BANK2_PGERR) || ((FLAG) == FLASH_FLAG_BANK2_WRPRTERR))
#else
	#define FLASH_FLAG_BSY                 ((uint32_t)0x00000001)  /*!< FLASH Busy flag */
	#define FLASH_FLAG_EOP                 ((uint32_t)0x00000020)  /*!< FLASH End of Operation flag */
	#define FLASH_FLAG_PGERR               ((uint32_t)0x00000004)  /*!< FLASH Program error flag */
	#define FLASH_FLAG_WRPRTERR            ((uint32_t)0x00000010)  /*!< FLASH Write protected error flag */
	#define FLASH_FLAG_OPTERR              ((uint32_t)0x00000001)  /*!< FLASH Option Byte error flag */

	#define FLASH_FLAG_BANK1_BSY                 FLASH_FLAG_BSY       /*!< FLASH BANK1 Busy flag*/
	#define FLASH_FLAG_BANK1_EOP                 FLASH_FLAG_EOP       /*!< FLASH BANK1 End of Operation flag */
	#define FLASH_FLAG_BANK1_PGERR               FLASH_FLAG_PGERR     /*!< FLASH BANK1 Program error flag */
	#define FLASH_FLAG_BANK1_WRPRTERR            FLASH_FLAG_WRPRTERR  /*!< FLASH BANK1 Write protected error flag */

	#define IS_FLASH_CLEAR_FLAG(FLAG) ((((FLAG) & (uint32_t)0xFFFFFFCA) == 0x00000000) && ((FLAG) != 0x00000000))
	#define IS_FLASH_GET_FLAG(FLAG)  (((FLAG) == FLASH_FLAG_BSY) || ((FLAG) == FLASH_FLAG_EOP) || \
									  ((FLAG) == FLASH_FLAG_PGERR) || ((FLAG) == FLASH_FLAG_WRPRTERR) || \
									  ((FLAG) == FLASH_FLAG_BANK1_BSY) || ((FLAG) == FLASH_FLAG_BANK1_EOP) || \
									  ((FLAG) == FLASH_FLAG_BANK1_PGERR) || ((FLAG) == FLASH_FLAG_BANK1_WRPRTERR) || \
									  ((FLAG) == FLASH_FLAG_OPTERR))
#endif

void write_word (uint32_t Address, uint32_t Data);

#endif /* FLASH_FLASH_H_ */
