#ifndef __CHASSIS_H__
#define __CHASSIS_H__


#include "rm_typedef.h"
#include "pid.h"
#include "dji_motor.h"
#include "filter.h"
#include "limiter.h"
#include "curve_mapping.h"



#define CHASSIS_TICK_TIME		1
#define ENCODER_TO_ANGLE(x) ((float)(x) / 8192.0f * 360.0f)



#define SQRT_2_OVER_2 		0.70710678f     // 斜对角速度分解系数（用于轮子计算）
#define YAW_ZERO_OFFSET_DEGREES 	75.0f	// yaw轴零位偏移角度
#define GIMBAL_YAW_ENCODER_MIDDLE1 0      // 0°方向
#define GIMBAL_YAW_ENCODER_MIDDLE2 4096   // 180°方向

// 运动学矩阵
static const float MECANUM_MATRIX[4][3] = {
    {-SQRT_2_OVER_2, SQRT_2_OVER_2, 1},    // 左前轮
    {SQRT_2_OVER_2, SQRT_2_OVER_2, 1},     // 右前轮
    {SQRT_2_OVER_2, -SQRT_2_OVER_2, 1},    // 左后轮
    {-SQRT_2_OVER_2, -SQRT_2_OVER_2, 1}    // 右后轮
};


#pragma pack(1)

typedef enum Chassis_Sport_Mode_Enum_T
{
	CHASSIS_FOLLOW = 0,		// 底盘跟随模式
	CHASSIS_SPIN		// 小陀螺旋转模式
} ChassisSportModeEnum;

typedef struct Chassis_Speed_Struct_T
{
	float vx;     // 前进速度 (m/s)
    float vy;     // 横向速度 (m/s)
    float vw;     // 旋转角速度 (rad/s)
} ChassisSpeedTypeDef;

typedef struct Chassis_Motion_Struct_T
{
	float angle;        // 运动方向角度 (度)
	float speed;       // 运动速度 (单位待定)
} ChassisMotionTypeDef;

typedef struct Chassis_Motor_Struct_T
{
	int16_t current_speed;	// 当前速度
	int16_t target_speed;	// 目标速度
	
	PidControlTypeDef pid_speed;	// 速度环PID控制器
} ChassisMotorTypeDef;

typedef struct Chassis_Motor_Yaw_Struct_T
{
	int16_t current_speed;	// 当前速度
	int16_t target_speed;	// 目标速度
	
	float target_angle;     // 目标角度
    float current_angle;    // 当前角度
	
	int32_t total_angle;	// 累计总角度（不带圈数）
	int32_t rotation_count;	// 累计总角度（带圈数）
	
	PidControlTypeDef pid_speed;	// 速度环PID控制器
	PidControlTypeDef pid_angle;	// 角度环PID控制器
} ChassisMotorYawTypeDef;

typedef struct Chassis_Follow_Struct_T
{
	float target_angle;		// 跟随目标角度
	float current_angle;	// 当前角度

	PidControlTypeDef pid_angle;	// 角度环PID控制器
} ChassisFollowTypeDef;

typedef struct Chassis_Struct_T
{
	uint8_t flag;
	uint32_t tick;
	
	ChassisMotorTypeDef chassis_motor[DJI_MOTOR_CHASSIS_RX_SUM];
	ChassisMotorYawTypeDef chassis_yaw_motor[DJI_MOTOR_CHASSIS_YAW_RX_SUM];
	ChassisFollowTypeDef chassis_follow;
	ChassisMotionTypeDef motion;	// 底盘运动信息
	ChassisSpeedTypeDef abs_chassis_speed;

	SpeedRampControlTypeDef x_ramp; // 速度x斜坡控制器
	SpeedRampControlTypeDef y_ramp; // 速度y斜坡控制器
	SpeedRampControlTypeDef gryo_ramp;	// 角速度斜坡控制器

	AngleRateLimitTypeDef angle_change;	// 角速度限制器
	
	CurveConfigTypeDef stick_coinfig;	// yaw轴映射曲线
} ChassisTypeDef;

#pragma pack()

extern ChassisTypeDef Chassis; 




/* *************** Chassis Function *************** */
void Chassis_Init(void);
void Chassis_Task(void);
void Chassis_Omni_Get_Data(void);
void Chassis_Yaw_Get_Data(void);
void Chassis_Timing_Callback(void);



#endif
