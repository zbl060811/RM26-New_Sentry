#ifndef __COMMUNICATE_H__
#define __COMMUNICATE_H__



#include "rm_typedef.h"
#include "bsp_can.h"



#define CAN_COMMUNICATE_TICK_TIME   1

// 本机ID：0X101     服务ID：0X100
#define CAN_COMMUNICATE_LOCAL_ID      0X101        
#define CAN_COMMUNICATE_SEND_ID       0X100


#pragma pack(1)


typedef enum Can_Communicate_Type_Enum_T
{
    CAN_MSG_NONE = 0x00,       // 无操作

    CAN_MSG_RC_LEFT = 0X01,    // 左遥控器信息(left_x, left_y)
    CAN_MSG_RC_RIGHT = 0X02,   // 右遥控器信息(right_x, right_y)

    CAN_MSG_EULAR = 0X03,    // 陀螺仪信息(欧拉角)
    CAN_MSG_STATUS = 0X04,   // 机器人状态信息(rc在线, 运动)
    CAN_MSG_VISION = 0X05    // 视觉信息
} CanCommunicateTypeEnum;

typedef struct Can_Communicate_Data_Struct_T
{
    uint8_t flag;
    uint32_t tick;

	int16_t rc_left_x;      // 遥控器left_x轴
    int16_t rc_left_y;      // 遥控器left_y轴
    
    int16_t rc_right_x;     // 遥控器right_x轴
    int16_t rc_right_y;     // 遥控器right_y轴
    
    float yaw_rate;      // 偏航角速度  
    float pitch;         // 俯仰角
    float yaw;           // 偏航角

	uint8_t rc_status;      // 遥控器状态 0：正常1：丢失
    uint8_t sport_mode;     // 运动模式
    uint8_t power_mode;     // 电源模式

    uint8_t target_found;     // 是否检测到目标
    uint8_t target_id;        // 目标ID
    int16_t vision_yaw;              // 目标相对yaw角
    int16_t vision_pitch;            // 目标相对pitch角
} CanCommunicateDataTypeDef;

typedef struct Can_Communicate_Struct_T
{
    CAN_HandleTypeDef *hcan;
    uint16_t TX_STD_ID;
    uint16_t RX_STD_ID;

    CanCommunicateDataTypeDef data;
} CanCommunicateTypeDef;



#pragma pack()


extern CanCommunicateTypeDef Can_Communicate;




/* *************** Communicate Function *************** */
void Communicate_Init(void);
void Communicate_Rx_Filter_Set(void);
char Communicate_Get_Data(CanCommunicateTypeDef *can, CAN_RxHeaderTypeDef *rxheader, uint8_t *rx_buffer);
void Communicate_Task(void);
void Communicate_Timing_Callback(void);
void Communicate_Data_Process(uint8_t *rx_buffer);
float Update_Yaw_Angle(float current_yaw, int yaw_mode);







#endif
