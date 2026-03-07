#include "pid.h"




void Pid_Init(PidControlTypeDef *pid)
{
	pid->pid_data.kp = 0;
	pid->pid_data.ki = 0;
	pid->pid_data.kd = 0;
	pid->pid_data.ki_max = 0;
	pid->pid_data.out_max = 0;
	
	pid->pid_data.error[0] = 0;
	pid->pid_data.error[1] = 0;
	pid->pid_data.target = 0;
	pid->pid_data.feedback = 0;
	
	pid->pid_data.p_out = 0;
	pid->pid_data.i_out = 0;
	pid->pid_data.d_out = 0;
	pid->pid_data.output = 0;
}


void Pid_Set(PidControlTypeDef *pid, float kp, float ki, float kd, float ki_max, float out_max)
{
	pid->pid_data.kp = kp;
	pid->pid_data.ki = ki;
	pid->pid_data.kd = kd;
	
	pid->pid_data.ki_max = ki_max;
	pid->pid_data.out_max = out_max;
}


float Pid_Calc(PidControlTypeDef *pid, float ref, float feedback)
{
    pid->pid_data.target = ref;
    pid->pid_data.feedback = feedback;
    pid->pid_data.error[1] = pid->pid_data.error[0];
    pid->pid_data.error[0] = pid->pid_data.target - pid->pid_data.feedback;
    
    pid->pid_data.p_out = pid->pid_data.kp * pid->pid_data.error[0];
    pid->pid_data.i_out += pid->pid_data.ki * pid->pid_data.error[0];
    pid->pid_data.d_out = pid->pid_data.kd * (pid->pid_data.error[0] - pid->pid_data.error[1]);

    if(pid->pid_data.i_out > pid->pid_data.ki_max)
        pid->pid_data.i_out = pid->pid_data.ki_max;
    else if(pid->pid_data.i_out < -(pid->pid_data.ki_max))
        pid->pid_data.i_out = -pid->pid_data.ki_max;    

    pid->pid_data.output = pid->pid_data.p_out + pid->pid_data.i_out + pid->pid_data.d_out;

    if(pid->pid_data.output > pid->pid_data.out_max)
        pid->pid_data.output = pid->pid_data.out_max;
    else if(pid->pid_data.output < -pid->pid_data.out_max)
        pid->pid_data.output = -pid->pid_data.out_max;

    return pid->pid_data.output;
}


