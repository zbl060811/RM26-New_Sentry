#ifndef __ROBOT_H__
#define __ROBOT_H__



#include "rm_typedef.h"



#define ROBOT_TICK_TIME      10



#pragma pack(1)


typedef enum Robot_Sport_Mode_Enum_T
{
    FOLLOW_GIMBAL = 0,      // 跟随云台
    GYROSCOPE,       // 小陀螺模式
} RobotSportModeEnum;

typedef enum Robot_Power_Mode_Enum_T
{
    POWER_MODE_LOW,   // 低电模式(30W)
    POWER_MODE_MID,   // 中电模式(60W)
    POWER_MODE_HIGH,  // 高电模式(100W)
} RobotPowerModeEnum;

typedef enum Robot_Control_Mode_Enum_T
{
    FIRE_MODE_MANUAL,   // 手动模式
    FIRE_MODE_AUTO,     // 自动模式
    FIRE_MODE_DEBUG,    // 调试模式
} RobotControlModeEnum;

typedef enum Robot_Fire_Mode_Enum_T
{
    FIRE_MODE_LOW_THREAT,   // 低威胁
    FIRE_MODE_MID_THREAT,   // 中威胁
    FIRE_MODE_HIGH_THREAT,  // 高威胁
} RobotFireModeEnum;

typedef struct Robot_Status_Strcut_T
{
    uint8_t sport_mode;     // 运动模式
    uint8_t power_mode;     // 电源策略
    uint8_t control_mode;   // 控制方式
    uint8_t fire_mode;      // 战斗模式

    uint16_t heat_power;    // 热量
} RobotStatusTypeDef;

typedef struct Robot_Struct_T
{
    uint8_t flag;
    uint32_t tick;

    RobotStatusTypeDef status;
} RobotTypeDef;






#pragma pack()

extern RobotTypeDef Robot;

/* *************** Robot Function *************** */
void Robot_Init(void);
void Robot_Task(void);
void Robot_Timing_Callback(void);
void Robot_Control(void);




#endif
