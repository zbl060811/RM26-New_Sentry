#ifndef __PUBLIC_CMD_H__
#define __PUBLIC_CMD_H__



#include "rm_typedef.h"
#include "communicate.h"




#define PUBLIC_CMD_TICK_TIME   5



#pragma pack(1)

typedef enum Public_Cmd_Type_Enum_T
{
    CAN_MSG_NONE = 0x00,       // 无操作
    CAN_MSG_RC = 0X01,      // 遥控器信息(right_x, right_y, left_x)
    CAN_MSG_EULAR = 0X02,   // 陀螺仪信息(欧拉角)
    CAN_MSG_STATUS = 0X03   // 机器人状态信息(rc在线, 运动)
} PublicCmdTypeEnum;


typedef struct Public_Rc_Struct_T
{
    PublicCmdTypeEnum cmd_type;   // 命令类型
    int16_t rc_right_x;     // 遥控器right_x轴
    int16_t rc_right_y;     // 遥控器right_y轴
	int16_t rc_left_x;      // 遥控器left_x轴
} PublicRcTypeDef; 

typedef struct Public_Eular_Struct_T
{
    PublicCmdTypeEnum cmd_type;   // 命令类型
    int16_t yaw_rate;     // yaw角速度
    int16_t pitch;        // 俯仰角
    int16_t yaw;          // 偏航角
} PublicEularTypeDef;

typedef struct Public_Status_Struct_T
{
    PublicCmdTypeEnum cmd_type;   // 命令类型
    uint8_t rc_status;        // 遥控器在线状态
    uint8_t sport_mode;       // 机器人运动模式
    uint8_t power_mode;       // 电源模式
} PublicStatusTypeDef;


typedef struct Public_Cmd_Struct_T
{

    CanCommunicateTypeDef can_com;
    PublicRcTypeDef rc_data;
    PublicEularTypeDef eular_data;
    PublicStatusTypeDef status_data;

    uint8_t flag;
    uint32_t tick;

} PublicCmdTypeDef;



#pragma pack()


extern PublicCmdTypeDef Public;


/* *************** Public Cmd Function *************** */
void Public_Cmd_Init(void);
void Public_Cmd_Task(void);
void Public_Cmd_Timing_Callback(void);
void Public_Send_Rc(void);
void Public_Send_Eular(void);
void Public_Send_Status(void);





#endif
