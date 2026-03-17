#ifndef __NX_TOPIC_H__
#define __NX_TOPIC_H__



#include "rm_typedef.h"
#include "bsp_usart.h"




#define NX_RX_BUFFER_LENGTH		32
#define NX_USE_USART            huart8
#define NX_USE_USART_RX_DMA	    hdma_uart8_rx

#define NX_RX_HEAD		0X51



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
	NX_HEART_LEN = 9,
	NX_NAVIGATION = 21,
} NxLenEnum;

// 心跳帧数据结构 (CMD=0x03)
typedef struct
{
    uint8_t seq;              // 序列号 (0-255 循环)
    uint32_t timestamp;       // 时间戳 (ms) - 小端格式
} NxHeartbeatFrameTypeDef;

// 组合帧数据结构 (对应CMD=0x04)
typedef struct
{
    uint8_t target_found;     // 是否检测到目标 (0=未检测, 1=检测到)
    uint8_t target_id;        // 目标ID
    float yaw;                // 航向角 (度°)
    float pitch;              // 俯仰角 (度°)
    uint16_t distance;        // 距离 (cm)
    uint8_t nav_mode;         // 导航模式 (0=停止, 1=导航, 2=跟随)
    int16_t target_x;         // 目标X坐标 (mm)
    int16_t target_y;         // 目标Y坐标 (mm)
    uint16_t target_yaw;      // 目标航向 (度°)
    uint16_t velocity;        // 期望速度 (cm/s)
} NxCombinedFrameTypeDef;

// 解析后的数据联合体
typedef struct
{
    NxCombinedFrameTypeDef combined;
    NxHeartbeatFrameTypeDef heartbeat;
} NxFrameDataStruct;

// 完整的解析结果
typedef struct
{
    uint8_t cmd;              // 命令字
    uint8_t valid;            // 数据是否有效 (0=无效, 1=有效)
    NxFrameDataStruct data;    // 解析后的数据
} NxParsedFrameTypeDef;

typedef struct
{
	uint8_t flag;
	uint8_t tick;
	
	uint8_t rx_buffer[NX_RX_BUFFER_LENGTH];
	NxParsedFrameTypeDef parsed_frame;  // 添加解析结果
} VisionTopicTyepDef;


extern VisionTopicTyepDef vision_topic;

#pragma pack()



void Nx_Topic_Init(VisionTopicTyepDef *topic);
void Nx_Topic_Data_Process(VisionTopicTyepDef *topic, uint8_t *buff, uint8_t size);
NxCombinedFrameTypeDef* Nx_GetCombinedFrame(VisionTopicTyepDef *topic);
NxHeartbeatFrameTypeDef* Nx_GetHeartbeatFrame(VisionTopicTyepDef *topic);
void Nx_Rx_Callback(VisionTopicTyepDef *topic, uint8_t *Buffer, uint8_t size);










#endif
