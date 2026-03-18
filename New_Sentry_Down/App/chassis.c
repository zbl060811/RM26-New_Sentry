#include "chassis.h"

#include "led.h"
#include "filter.h"
#include "communicate.h"

#include "bsp_tim.h"




ChassisTypeDef Chassis;


/*

整车电机分布

M1	CAP	 M2

	 O

M4	BAT	 M3
	
*/


void Chassis_Init(void)
{
	Chassis.flag = 1;
	
	// 底盘电机PID初始化
	Pid_Init(&Chassis.chassis_motor[DJI_MOTOR_M3508_CHASSIS_RX_1].pid_speed);
	Pid_Set(&Chassis.chassis_motor[DJI_MOTOR_M3508_CHASSIS_RX_1].pid_speed, 16, 0, 0, 8000, 15000);
	
	Pid_Init(&Chassis.chassis_motor[DJI_MOTOR_M3508_CHASSIS_RX_2].pid_speed);
	Pid_Set(&Chassis.chassis_motor[DJI_MOTOR_M3508_CHASSIS_RX_2].pid_speed, 16, 0, 0, 8000, 15000);
	
	Pid_Init(&Chassis.chassis_motor[DJI_MOTOR_M3508_CHASSIS_RX_3].pid_speed);
	Pid_Set(&Chassis.chassis_motor[DJI_MOTOR_M3508_CHASSIS_RX_3].pid_speed, 16, 0, 0, 8000, 15000);
	
	Pid_Init(&Chassis.chassis_motor[DJI_MOTOR_M3508_CHASSIS_RX_4].pid_speed);
	Pid_Set(&Chassis.chassis_motor[DJI_MOTOR_M3508_CHASSIS_RX_4].pid_speed, 16, 0, 0, 8000, 15000);
	
	// 云台YAW电机PID初始化
	Pid_Init(&Chassis.chassis_yaw_motor[DJI_MOTOR_6020_CHASSIS_YAW_RX_1].pid_speed);
	Pid_Set(&Chassis.chassis_yaw_motor[DJI_MOTOR_6020_CHASSIS_YAW_RX_1].pid_speed, 3000, 0, 0, 3000, 20000);
	
	Pid_Init(&Chassis.chassis_yaw_motor[DJI_MOTOR_6020_CHASSIS_YAW_RX_1].pid_angle);
	Pid_Set(&Chassis.chassis_yaw_motor[DJI_MOTOR_6020_CHASSIS_YAW_RX_1].pid_angle, 0.5, 0, 0, 3000, 20000);

	// 底盘跟随电机PID初始化
	Pid_Init(&Chassis.chassis_follow.pid_angle);
	Pid_Set(&Chassis.chassis_follow.pid_angle, 30, 0, 0, 0, 10000);

	// 初始化yaw曲线配置（立方曲线+死区） 
    Curve_Config_Init(&Chassis.stick_coinfig,
                       CURVE_CUBIC_DEAD,
                       0.0f,        // param1未用
                       0.0f,        // param2未用
                       0.8f,        // 输出缩放比例
                       0.0f);      // 死区大小

	Chassis.x_ramp.max_acceleration = 10000;
	Chassis.x_ramp.max_deceleration = 8000;
	Chassis.x_ramp.max_speed = 4000;

	Chassis.y_ramp.max_acceleration = 10000;
	Chassis.y_ramp.max_deceleration = 8000;
	Chassis.y_ramp.max_speed = 4000;

	Chassis.gryo_ramp.max_acceleration = 10000;
	Chassis.gryo_ramp.max_deceleration = 10000;
	Chassis.gryo_ramp.max_speed = 5000;

}


void Chassis_Omni_Get_Data(void)
{
	for(uint8_t i = 0;i < DJI_MOTOR_CHASSIS_RX_SUM;i++){
		Chassis.chassis_motor[i].current_speed = DJI_Motor[DJI_CHASSIS_GROUP].motor_data[i].speed;
	}
}


void Chassis_Yaw_Get_Data(void)
{
	for(uint8_t i = 0;i < DJI_MOTOR_CHASSIS_YAW_RX_SUM;i++){
		Chassis.chassis_yaw_motor[i].current_speed = DJI_Motor[DJI_GIMBAL_GROUP].motor_data[i].speed;
		Chassis.chassis_yaw_motor[i].current_angle = ENCODER_TO_ANGLE(DJI_Motor[DJI_GIMBAL_GROUP].motor_data[i].angle);
	}
}



void Chassis_Omni_Calc(void)
{
	float dt = CHASSIS_TICK_TIME / 1000.0f;

	float target_x = 0, target_y = 0, omega = 0;

	float angle_offset = Chassis.chassis_yaw_motor[DJI_MOTOR_6020_CHASSIS_YAW_RX_1].current_angle + 43.0f;
    
	float yaw = Update_Yaw_Angle(angle_offset, 1);
    float normalized_yaw = fmodf(angle_offset, 360.0f);
    if (normalized_yaw < 0)
        normalized_yaw += 360.0f;

	const float current_yaw_rad = normalized_yaw * PI / 180.0f;
    const float cos_yaw = cosf(current_yaw_rad);
    const float sin_yaw = sinf(current_yaw_rad);

	Pid_Calc(&Chassis.chassis_follow.pid_angle, yaw, 0);

	switch(Can_Communicate.data.sport_mode){
		case CHASSIS_FOLLOW:
			target_x = -Can_Communicate.data.rc_right_y * Chassis.x_ramp.max_speed;
			target_y = Can_Communicate.data.rc_right_x * Chassis.y_ramp.max_speed;
			omega = Apply_Speed_Ramp(&Chassis.gryo_ramp, Chassis.chassis_follow.pid_angle.pid_data.output, dt);
		break;

		case CHASSIS_SPIN:
			target_x = -Can_Communicate.data.rc_right_y * Chassis.x_ramp.max_speed;
			target_y = Can_Communicate.data.rc_right_x * Chassis.y_ramp.max_speed;
			omega = Apply_Speed_Ramp(&Chassis.gryo_ramp, Chassis.gryo_ramp.max_speed, dt);
		break;
	}
	
	target_x = Apply_Speed_Ramp(&Chassis.x_ramp, target_x, dt);
	target_y = Apply_Speed_Ramp(&Chassis.y_ramp, target_y, dt);
	
	
	Chassis.abs_chassis_speed.vx = cos_yaw * target_x - sin_yaw * target_y;
	Chassis.abs_chassis_speed.vy = sin_yaw * target_x + cos_yaw * target_y;
	Chassis.abs_chassis_speed.vw = omega;
	
	for (int i = 0; i < 4; i++)
    {
        Chassis.chassis_motor[i].target_speed = (int16_t)(Chassis.abs_chassis_speed.vx * MECANUM_MATRIX[i][0] +
														  Chassis.abs_chassis_speed.vy * MECANUM_MATRIX[i][1] +
														  Chassis.abs_chassis_speed.vw * MECANUM_MATRIX[i][2]);
    } 
	
	for(uint8_t i = 0;i < DJI_MOTOR_CHASSIS_RX_SUM;i++){
		Chassis.chassis_motor[i].pid_speed.pid_data.output = Pid_Calc(&Chassis.chassis_motor[i].pid_speed, Chassis.chassis_motor[i].target_speed, Chassis.chassis_motor[i].current_speed);
	}
	
	Dji_Motor_Set_Current(DJI_CHASSIS_GROUP, DJI_MOTOR_TX_1_4,  Chassis.chassis_motor[0].pid_speed.pid_data.output,
																Chassis.chassis_motor[1].pid_speed.pid_data.output,
																Chassis.chassis_motor[2].pid_speed.pid_data.output,
																Chassis.chassis_motor[3].pid_speed.pid_data.output);
}


void Chassis_Yaw_Calc(void)
{
	static uint8_t first_enter = 1;
	static int16_t yaw_rotation_count = 0;
	static float previous_yaw = 0;
	static float continuous_yaw = 0;

	float yaw_difference = Can_Communicate.data.yaw - previous_yaw;
	if(yaw_difference > 180.0f){
		yaw_rotation_count -= 1;
	}else if(yaw_difference < -180.0f){
		yaw_rotation_count += 1;
	}

	continuous_yaw = yaw_rotation_count * 360.0f + Can_Communicate.data.yaw;
	previous_yaw = Can_Communicate.data.yaw;

	Chassis.chassis_yaw_motor[DJI_MOTOR_6020_CHASSIS_YAW_RX_1].current_angle = continuous_yaw;

	if(first_enter){
		Chassis.chassis_yaw_motor[DJI_MOTOR_6020_CHASSIS_YAW_RX_1].target_angle = Chassis.chassis_yaw_motor[DJI_MOTOR_6020_CHASSIS_YAW_RX_1].current_angle;
		first_enter = 0;
	}

	float target_angle_increment = 0;
	if(Can_Communicate.data.ctrl_mode == 1)
	{
		if(Can_Communicate.data.target_found == 1)
		{
			target_angle_increment = (Can_Communicate.data.vision_yaw * 0.03f);
			// target_angle_increment = -(Can_Communicate.data.rc_left_x * 0.3f);
		}
		else
		{
			target_angle_increment = -(Can_Communicate.data.rc_left_x * 0.4f);
		}
	}
	else
	{
		target_angle_increment = -(Can_Communicate.data.rc_left_x * 0.4f);
	}

	Chassis.chassis_yaw_motor[DJI_MOTOR_6020_CHASSIS_YAW_RX_1].target_angle += target_angle_increment;

	Chassis.chassis_yaw_motor[DJI_MOTOR_6020_CHASSIS_YAW_RX_1].pid_angle.pid_data.output = Pid_Calc(&Chassis.chassis_yaw_motor[DJI_MOTOR_6020_CHASSIS_YAW_RX_1].pid_angle, 
																									 Chassis.chassis_yaw_motor[DJI_MOTOR_6020_CHASSIS_YAW_RX_1].target_angle,
																									 Chassis.chassis_yaw_motor[DJI_MOTOR_6020_CHASSIS_YAW_RX_1].current_angle);

																					
	Chassis.chassis_yaw_motor[DJI_MOTOR_6020_CHASSIS_YAW_RX_1].current_speed = Can_Communicate.data.yaw_rate * 0.1f;

	Chassis.chassis_yaw_motor[DJI_MOTOR_6020_CHASSIS_YAW_RX_1].pid_speed.pid_data.output = Pid_Calc(&Chassis.chassis_yaw_motor[DJI_MOTOR_6020_CHASSIS_YAW_RX_1].pid_speed, 
																						Chassis.chassis_yaw_motor[DJI_MOTOR_6020_CHASSIS_YAW_RX_1].pid_angle.pid_data.output,
																						Chassis.chassis_yaw_motor[DJI_MOTOR_6020_CHASSIS_YAW_RX_1].current_speed);

	Dji_Motor_Set_Current(DJI_GIMBAL_GROUP, DJI_MOTOR_TX_1_4, Chassis.chassis_yaw_motor[DJI_MOTOR_6020_CHASSIS_YAW_RX_1].pid_speed.pid_data.output, 
																0, 
																0,
																0);											
}


void Chassis_Task(void)
{
	if(!Chassis.flag || Chassis.tick < CHASSIS_TICK_TIME){		
		return;
	}
	
	Chassis.tick = 0;
	
	
	Chassis_Omni_Get_Data();
	Chassis_Yaw_Get_Data();
	
 	Chassis_Omni_Calc();
	Chassis_Yaw_Calc();
}



void Chassis_Timing_Callback(void)
{
	if(Chassis.tick < CHASSIS_TICK_TIME){
		Chassis.tick++;
	}
}


float Update_Yaw_Angle(float current_yaw, int yaw_mode)
{
    static float previous_yaw = 0;
    static int rotation_count = 0;
    static float absolute_angle = 0;

    if (current_yaw - previous_yaw > 180)
    {
        rotation_count--;
    }
    else if (current_yaw - previous_yaw < -180)
    {
        rotation_count++;
    }

    absolute_angle = ((float)rotation_count * 360) + current_yaw;
    previous_yaw = current_yaw;

    float offset;
    switch (yaw_mode)
    {
    case 1: // 两基准点模式 (0°, 180°)
    {
        int base_point = (int)(absolute_angle / 180.0f) * 180;
        offset = absolute_angle - base_point;

        if (offset > 90.0f)
        {
            offset -= 180.0f;
        }
        else if (offset < -90.0f)
        {
            offset += 180.0f;
        }
        break;
    }
    // case 2: // 四基准点模式 (0°, 90°, 180°, 270°)
    // {
    //     int base_point = (int)(absolute_angle / 90.0f) * 90;
    //     offset = absolute_angle - base_point;
    //     if (offset > 45.0f)
    //     {
    //         offset -= 90.0f;
    //     }
    //     else if (offset < -45.0f)
    //     {
    //         offset += 90.0f;
    //     }
    //     break;
    // }
    // default: // 单一基准点模式 (0°)
    //     offset = absolute_angle;
    //     break;
     }

    return offset;
}


