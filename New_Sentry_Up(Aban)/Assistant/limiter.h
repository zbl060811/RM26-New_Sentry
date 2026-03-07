#ifndef __LIMITER_H__
#define __LIMITER_H__


#include "rm_typedef.h"



#pragma pack(1)

typedef struct Angle_Limit_Struct_T
{
	float min_angle;		// 最小角度限制(度)
	float max_angle;		// 最大角度限制(度)
	uint8_t enabled;		// 是否启用限制
	float soft_limit;		// 软限制，接近硬限制时减速
} AngleLimitTypeDef;

typedef struct Speed_Ramp_Control_Struct_T
{
	float current_speed;		// 当前速度
	float max_speed;			// 最大速度限制
	float max_acceleration;		// 最大加速度
	float max_deceleration;		// 最大减速度
} SpeedRampControlTypeDef;

typedef struct Angle_Rate_Limit_Struct_T
{
	float last_angle;       // 上一次的角度值
    float max_rate;         // 最大变化率 (度/秒)
    float output_angle;     // 限制后的输出角度
} AngleRateLimitTypeDef;



#pragma pack()


/* *************** Limiter Function *************** */

void Angle_Rate_Limiter_Init(AngleRateLimitTypeDef *limiter, float max_rate);
float Angle_Rate_Limiter_Update(AngleRateLimitTypeDef *limiter, float input_angle, float dt);

void Angle_Limit_Init(AngleLimitTypeDef *limit, float min_angle, float max_angle, float soft_limit, uint8_t enabled);
float Angle_Limit_Apply(float input_angle, const AngleLimitTypeDef *limit);
int8_t Angle_Limit_Check(const AngleLimitTypeDef *limit, float angle, float *error);
float Angle_Limit_SoftLimit(float input_angle, const AngleLimitTypeDef *limit, float current_angle);



#endif

