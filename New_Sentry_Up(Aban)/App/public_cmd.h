#ifndef __PUBLIC_CMD_H__
#define __PUBLIC_CMD_H__



#include "rm_typedef.h"
#include "communicate.h"




#define PUBLIC_CMD_TICK_TIME        3
#define PUBLIC_CMD_MAX_DATA_LEN     8  // CAN数据帧最大长度



#pragma pack(1)

typedef enum Public_Cmd_Type_Enum_T
{
    CAN_MSG_NONE = 0x00,       // 无操作

    CAN_MSG_RC_LEFT = 0X01,    // 左遥控器信息(left_x, left_y)
    CAN_MSG_RC_RIGHT = 0X02,   // 右遥控器信息(right_x, right_y)
    CAN_MSG_VISION = 0X03,      // 视觉信息
    CAN_MSG_HEAT = 0X04,       // 热量信息 
} PublicCmdTypeEnum;


typedef struct Public_Rc_Left_Struct_T
{
    PublicCmdTypeEnum cmd_type;   // 命令类型
	int16_t rc_left_x;        // 遥控器left_x轴
    int16_t rc_left_y;        // 遥控器left_y轴
    uint8_t rc_status;        // 遥控器在线状态
    uint8_t sport_mode;       // 机器人运动模式
    uint8_t ctrl_mode;        // 控制模式
} PublicRcLeftTypeDef;

typedef struct Public_Rc_Right_Struct_T
{
    PublicCmdTypeEnum cmd_type;   // 命令类型
	int16_t rc_right_x;      // 遥控器right_x轴
    int16_t rc_right_y;      // 遥控器right_y轴
    int16_t yaw_rate;        // yaw角速度
    uint8_t target_found;     // 是否检测到目标
} PublicRcRightTypeDef;

typedef struct Public_Vision_Struct_T
{
    PublicCmdTypeEnum cmd_type;   // 命令类型
    uint8_t target_id;        // 目标ID
    int16_t vision_yaw;       // 目标相对yaw角
    int16_t vision_pitch;     // 目标相对pitch角
    int16_t yaw;             // 偏航角
} PublicVisionTypeDef;


typedef struct Public_Cmd_Struct_T
{
    CanCommunicateTypeDef can_com;
    
    PublicRcLeftTypeDef rc_left_data;
    PublicRcRightTypeDef rc_right_data;    
    PublicVisionTypeDef vision_data;

    uint8_t flag;
    uint32_t tick;

} PublicCmdTypeDef;



#pragma pack()


extern PublicCmdTypeDef Public;


/* *************** Public Cmd Function *************** */
void Public_Cmd_Init(void);
void Public_Send_RcLeft(void);
void Public_Send_RcRight(void);
void Public_Send_Eular(void);
void Public_Send_Status(void);
void Public_Send_Vision(void);



void Public_Cmd_Task(void);
void Public_Cmd_Timing_Callback(void);



#endif
