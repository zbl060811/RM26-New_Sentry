#include "gimbal.h"
#include "dji_motor.h"
#include "config.h"
#include "at9s.h"
#include "dr16.h"
#include "hi14.h"
#include "mg_motor.h"




GimbalTypeDef Gimbal;



void Gimbal_Init(void)
{
	Gimbal.flag = 1;
	
    // 初始化yaw电机PID参数
	Pid_Init(&Gimbal.yaw_motor.pid_speed);
	Pid_Set(&Gimbal.yaw_motor.pid_speed, 12, 0, 0, 0, 20000);
	
	Pid_Init(&Gimbal.yaw_motor.pid_angle);
	Pid_Set(&Gimbal.yaw_motor.pid_angle, 8, 0, 0, 0, 20000);

    // 初始化yaw曲线配置（立方曲线+死区） 
    Curve_Config_Init(&Gimbal.yaw_motor.stick_coinfig,
                       CURVE_CUBIC_DEAD,
                       0.0f,        // param1未用
                       0.0f,        // param2未用
                       0.8f,        // 输出缩放比例
                       0.0f);       // 死区大小

    // 初始化角度变化率限制器
    Angle_Rate_Limiter_Init(&Gimbal.yaw_motor.angle_change, 90.0f);
	
	// 初始化角度限制
	Angle_Limit_Init(&Gimbal.yaw_motor.angle_limit, YAW_MIN_ANGLE, YAW_MAX_ANGLE, YAW_SOFT_LIMIT, 1);
    


	
} 


void Gimbal_Yaw_Get_Data(void)
{
	Gimbal.yaw_motor.current_speed = DJI_Motor[DJI_GIMBAL_GROUP].motor_data[DJI_MOTOR_GIMBAL_GM6020_YAW_RX_1].speed;
	// Gimbal.yaw_motor.current_angle = msp(DJI_Motor[DJI_GIMBAL_GROUP].motor_data[DJI_MOTOR_GIMBAL_GM6020_YAW_RX_1].angle, 0, 8191, -180, 180);

	Gimbal.yaw_motor.current_angle = msp(DJI_Motor[DJI_GIMBAL_GROUP].motor_data[DJI_MOTOR_GIMBAL_GM6020_YAW_RX_1].angle, 0, 8191, -180, 180);

}


float Gimbal_Yaw_Angle_Update(GimbalYawMotorTypeDef* motor, float target_angle_degrees, float zero_offset)
{
    // 调整目标角度：考虑云台零位偏移
    float adjusted_target = target_angle_degrees + zero_offset;
    
    // 计算角度误差
    float angle_error = motor->current_angle - adjusted_target;
    
    // 角度归一化处理
    if(angle_error > 180.0f) angle_error -= 360.0f;
    if(angle_error < -180.0f) angle_error += 360.0f;
    
    
    // 角度环PID计算
    float encoder_error = angle_error * (8191.0f / 360.0f);
    float speed_target = Pid_Calc(&motor->pid_angle, 0, encoder_error);
    
    // 速度目标限幅
    const float MAX_SPEED_TARGET = 4000.0f;
    if(speed_target > MAX_SPEED_TARGET) speed_target = MAX_SPEED_TARGET;
    if(speed_target < -MAX_SPEED_TARGET) speed_target = -MAX_SPEED_TARGET;
    
    return speed_target;
}


int16_t Gimbal_Yaw_Speed_Update(GimbalYawMotorTypeDef* motor, int16_t target_speed)
{
    // 速度环PID计算
    int16_t current_output = Pid_Calc(&motor->pid_speed, target_speed, motor->current_speed);
    return current_output;
}


void Gimbal_Yaw_Calc(void)
{
//    float dt = GIMBAL_TICK_TIME / 1000.0f;
    
    #if CONFIG_USE_REMOTE
        // float rc_input = Dr16.data.RC_Value.CH2 * 0.2;
    #else
        float rc_input = At9s.at9s_rc.left_x;
    #endif

    float rc_input = 0;
	
    // 立方曲线降低小摇杆灵敏度
    rc_input = Curve_MapWithDeadzone(rc_input, &Gimbal.yaw_motor.stick_coinfig);
    
    float desired_angle = (-rc_input * 180.0f) + YAW_ZERO_OFFSET_DEGREES;
    
	// 角度硬限制
	desired_angle = Angle_Limit_Apply(desired_angle, &Gimbal.yaw_motor.angle_limit);
	
	// 角度软限制
//	desired_angle = Angle_Limit_SoftLimit(desired_angle, 
//                                          &Gimbal.yaw_motor.angle_limit, 
//                                          Gimbal.yaw_motor.current_angle);
    
    // 使用角度变化率限制器
    // float limited_angle = Angle_Rate_Limiter_Update(&Gimbal.yaw_motor.angle_change, 
    //                                                 desired_angle, 
    //                                                 dt);
    
	// 最终确保角度限制
//    limited_angle = Angle_Limit_Apply(limited_angle, &Gimbal.yaw_motor.angle_limit); 
	
    Gimbal.yaw_motor.target_angle = desired_angle;
    Gimbal.yaw_motor.target_speed = Gimbal_Yaw_Angle_Update(&Gimbal.yaw_motor, 
                                                             Gimbal.yaw_motor.target_angle,
                                                             0);
    
    int16_t current_output = Gimbal_Yaw_Speed_Update(&Gimbal.yaw_motor, (int16_t)Gimbal.yaw_motor.target_speed);
    Gimbal.yaw_motor.pid_speed.pid_data.output = current_output;
    
    Dji_Motor_Set_Current(DJI_GIMBAL_GROUP, DJI_MOTOR_TX_1_4, Gimbal.yaw_motor.pid_speed.pid_data.output, 
                                                              0, 
                                                              0,
                                                              0);   
}


void Gimbal_Pitch_Calc(void)
{
    /*
        使用模式：多圈位置闭环控制命令2（可控速度）
        向上最大：200
        中心点：  0
        向下最大：-200
    */
    #if CONFIG_USE_REMOTE
        float rc_input = Dr16.data.RC_Value.CH3;
    #else
        float rc_input = At9s.at9s_rc.left_y;
    #endif

    mg_motor.params.angle += rc_input * 1000;        // 目标角度增量，单位0.01度
    MG_Motor_PositionControl(&mg_motor);
}

void Gimbal_Task(void)
{
	if(!Gimbal.flag || Gimbal.tick < GIMBAL_TICK_TIME){ 
		return;
	}
	
	Gimbal.tick = 0;		// 清除定时周期

	
	Gimbal_Yaw_Get_Data();

	Gimbal_Pitch_Calc();
	Gimbal_Yaw_Calc();
}


void Gimbal_Timing_Callback(void)
{
	if(Gimbal.tick < GIMBAL_TICK_TIME){
		Gimbal.tick++;
	}
}



