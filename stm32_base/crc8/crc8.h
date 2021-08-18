
#ifndef CRC8_H_
#define CRC8_H_

#include <global_defines.h>

//uint8_t crc8(uint8_t *ptr,uint8_t cnt);
uint8_t crc8(volatile uint8_t *ptr, uint8_t cnt);
uint8_t crc8_parts(uint8_t previous_crc, uint8_t *ptr, uint8_t cnt);

#endif /* CRC8_H_ */
