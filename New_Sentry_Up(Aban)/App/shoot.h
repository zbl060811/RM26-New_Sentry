#ifndef __SHOOT_H__
#define __SHOOT_H__



#include "rm_typedef.h"
#include "pid.h"


#define SHOOT_TICK_TIME		1



#pragma pack(1)


typedef enum Shoot_Mode_Enum_T
{
	SHOOT_STOP,		// 暂停
	SHOOT_FIRE,		// 发射
	SHOOT_BACK,		// 退弹
	SHOOT_ERROR		// 错误(过热/堵转)
} ShootModeEnum;


typedef struct Shoot_Friction_Motor_Struct_T
{
	float target_speed;
	float current_speed;

	PidControlTypeDef speed_pid;
} ShootFrictionMotorTypeDef;

typedef struct Shoot_Dial_Motor_Struct_T
{
	float target_speed;
	float current_speed;

	PidControlTypeDef speed_pid;
} ShootDialMotorTypeDef;

typedef struct Shoot_Struct_T
{
	uint8_t flag;
	uint32_t tick;

	ShootFrictionMotorTypeDef friction_motor[2];	// 0:右电机 1：左电机
	ShootDialMotorTypeDef dial_motor;		// 拨盘电机
} ShootTypeDef;



#pragma pack()


extern ShootTypeDef Shoot;



/* *************** Shoot Function *************** */
void Shoot_Init(void);
void Shoot_Friction_Get_Data(void);
void Shoot_Dial_Get_Data(void);
void Shoot_Task(void);
void Shoot_Timing_Callback(void);



#endif
