#ifndef __NX_TOPIC_H__
#define __NX_TOPIC_H__



#include "rm_typedef.h"
#include "bsp_usart.h"




#define NX_RX_BUFFER_LENGTH		32
#define NX_USE_USART            huart8
#define NX_USE_USART_RX_DMA	    hdma_uart8_rx

#define NX_RX_TAIL		0X51




#pragma pack(1)


typedef enum
{
	NX_TYPE_VISION = 0x02,
	NX_TYPE_HEART = 0x03,
	NX_TYPE_NAVIGATION = 0x04
} NxTypeEnum;

typedef enum
{
	NX_VISION_LEN,
	NX_HEART_LEN,
	NX_NAVIGATION = 21,
} NxLenEnum;


typedef struct
{
	uint8_t flag;
	uint8_t tick;
	
	uint8_t rx_buffer[NX_RX_BUFFER_LENGTH];
} VisionTopicTyepDef;



#pragma pack()







#endif
