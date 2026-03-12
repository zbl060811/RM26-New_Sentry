#ifndef __NX_TOPIC_H__
#define __NX_TOPIC_H__



#include "bsp_uart.h"



#define NX_RX_BUFFER_LENGTH		32
#define NX_USE_USART            huart4
#define NX_USE_USART_RX_DMA	    hdma_uart4_rx

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
	NX_VISION_LEN ,
	NX_HEART_LEN,
	NX_NAVIGATION = 21,
} NxLenEnum;


typedef struct 
{
    uint8_t  target_found;      // 是否检测到目标 (0=未检测, 1=检测到)
    uint8_t  target_id;         // 目标ID
    int16_t  yaw;               // 航向角 (度°)
    int16_t  pitch;             // 俯仰角 (度°)
    uint16_t distance;          // 距离 (cm)
    uint8_t  nav_mode;          // 导航模式 (0=停止, 1=导航, 2=跟随)
    int16_t  target_x;          // 目标X坐标 (mm)
    int16_t  target_y;          // 目标Y坐标 (mm)
    uint16_t target_yaw;        // 目标航向角 (度°)
    uint16_t velocity;          // 期望速度 (mm/s)
} NxCombinedFrame_t;

typedef struct
{
    uint8_t  seq;            // 序列号
    uint32_t timestamp;      // 时间戳 (ms)
} NxHeartbeatFrame_t;

typedef struct
{
	uint8_t flag;
	uint8_t tick;
	
	uint8_t rx_buffer[NX_RX_BUFFER_LENGTH];
    NxCombinedFrame_t combined;
    NxHeartbeatFrame_t heartbeat;
} NxTopicTyepDef;



#pragma pack()



void Nx_Topic_Init(void);
void Nx_Parse_Data(NxTopicTyepDef *nx_data, uint8_t *data, uint16_t len);






#endif

