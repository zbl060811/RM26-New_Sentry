#ifndef __PID_H__
#define __PID_H__



#include "rm_typedef.h"




#pragma pack(1)


typedef enum Pid_State_Enum_T
{
	PID_DISABLE,
	PID_ENABLE
} PidStateEnum;

typedef struct Pid_Data_Struct_T
{
	float kp;            // 比例
    float ki;            // 积分
    float kd;            // 微分
    float ki_max;        // 积分限幅
    float out_max;       // 输出限幅

    float error[2];      // 误差值,error[0]为当前误差,error[1]为上一次的误差
    float target;        // 目标值
    float feedback;      // 反馈值

    float p_out;         // P输出
    float i_out;         // I输出
    float d_out;         // D输出
    float output;        // 总输出
} PidDataTypeDef;

typedef struct Pid_Control_Struct_T
{
	PidStateEnum pid_state;
	PidDataTypeDef pid_data;
} PidControlTypeDef;




#pragma pack()



/* *************** Pid Function *************** */
void Pid_Init(PidControlTypeDef *pid);
void Pid_Set(PidControlTypeDef *pid, float kp, float ki, float kd, float ki_max, float out_max);
float Pid_Calc(PidControlTypeDef *pid, float ref, float feedback);





#endif
