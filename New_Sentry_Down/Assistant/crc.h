#ifndef __CRC_H__
#define __CRC_H__


#include "rm_typedef.h"










// CRC8校验函数
uint8_t get_CRC8_check_sum(uint8_t *pchMessage, uint16_t dwLength, uint8_t ucCRC8);
uint8_t verify_CRC8_check_sum(uint8_t *pchMessage, uint16_t dwLength);
void append_CRC8_check_sum(uint8_t *pchMessage, uint16_t dwLength);

// CRC16校验函数
uint16_t get_CRC16_check_sum(uint8_t *pchMessage, uint32_t dwLength, uint16_t wCRC);
uint16_t verify_CRC16_check_sum(uint8_t *pchMessage, uint32_t dwLength);
void append_CRC16_check_sum(uint8_t *pchMessage, uint32_t dwLength);

#endif
