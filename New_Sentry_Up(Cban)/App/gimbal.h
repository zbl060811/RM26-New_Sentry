#ifndef __GIMBAL_H__
#define __GIMBAL_H__




#include "rm_typedef.h"
#include "pid.h"
#include "mg_motor.h"
#include "curve_mapping.h"
#include "limiter.h"
#include "filter.h"



#define GIMBAL_TICK_TIME	10
#define ENCODER_TO_ANGLE(x) ((float)(x) / 8191.0f * 360.0f)

#define YAW_ZERO_OFFSET_DEGREES 	-60.0f	// yaw轴零位偏移角度
#define YAW_MIN_ANGLE		-90.0f		// 最小角度（左转极限）
#define YAW_MAX_ANGLE		-30.0f		// 最大角度（右转极限）
#define YAW_SOFT_LIMIT		10.0f		// 软限制边界（离硬限制10度开始减速）


// #define YAW_MIN_ANGLE		-20.0f		// 最小角度（左转极限）
// #define YAW_MAX_ANGLE		10.0f		// 最大角度（右转极限）


#pragma pack(1)



typedef struct Gimbal_Yaw_Motor_Struct_T
{
	int16_t current_speed;		// 当前速度
	int16_t target_speed;		// 目标速度
	
	float current_angle;	// 当前角度
	float target_angle;		// 目标角度
	float last_angle;		// 先前角度
	
	PidControlTypeDef pid_speed;	// 速度环PID控制器
	PidControlTypeDef pid_angle;	// 角度环PID控制器
	
	SpeedRampControlTypeDef speed_ramp;		// 速度渐变控制器
	AngleRateLimitTypeDef angle_change;	// 角度变化率限制器
	AngleLimitTypeDef angle_limit;		// 角度限制
	CurveConfigTypeDef stick_coinfig;	// yaw轴映射曲线
} GimbalYawMotorTypeDef;


typedef struct Gimbal_Struct_T
{
	uint8_t flag;
	uint32_t tick;
	
	GimbalYawMotorTypeDef yaw_motor;	
} GimbalTypeDef;



#pragma pack()



extern GimbalTypeDef Gimbal;


/* *************** Gimbal Function *************** */
void Gimbal_Init(void);
void Gimbal_Yaw_Get_Data(void);
float Gimbal_Yaw_Angle_Update(GimbalYawMotorTypeDef* motor, float target_angle_degrees, float zero_offset);
int16_t Gimbal_Yaw_Speed_Update(GimbalYawMotorTypeDef* motor, int16_t target_speed);
void Gimbal_Yaw_Calc(void);





void Gimbal_Task(void);
void Gimbal_Timing_Callback(void);





#endif

