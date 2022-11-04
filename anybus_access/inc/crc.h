/*
 * crc.h
 *
 *  Created on: Mar 18, 2019
 *      Author: hinoeng
 */

#ifndef CRC_H_
#define CRC_H_
#include <stdint.h>

#define CRC_WORD_LEN_IN_WORDS 2
uint16_t CRC_Crc16(uint8_t* pbBufferStart, uint16_t iLength);
uint32_t CRC_crc32( uint16_t* piBufferStart, uint16_t iLength );

#endif /* CRC_H_ */
