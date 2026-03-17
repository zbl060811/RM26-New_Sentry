#include "shoot.h"
#include "robot.h"
#include "dji_motor.h"
#include "dr16.h"



ShootTypeDef Shoot;


void Shoot_Init(void)
{
	Shoot.flag = 1;

    Pid_Init(&Shoot.friction_motor[0].speed_pid);
    Pid_Set(&Shoot.friction_motor[0].speed_pid, 2, 0, 0, 0, 10000);

    Pid_Init(&Shoot.friction_motor[1].speed_pid);
    Pid_Set(&Shoot.friction_motor[1].speed_pid, 2, 0, 0, 0, 10000);

    Pid_Init(&Shoot.dial_motor.speed_pid);
    Pid_Set(&Shoot.dial_motor.speed_pid, 1.5, 0, 0, 0, 10000);
}


void Shoot_Friction_Get_Data(void)
{
    Shoot.friction_motor[0].current_speed = DJI_Motor[DJI_SHOOT_GROUP].motor_data[DJI_MOTOR_SHOOT_LEFT_M2006_RX_1].speed;
    Shoot.friction_motor[1].current_speed = DJI_Motor[DJI_SHOOT_GROUP].motor_data[DJI_MOTOR_SHOOT_RIGHT_M2006_RX_2].speed;
}


void Shoot_Dial_Get_Data(void)
{
    Shoot.dial_motor.current_speed = DJI_Motor[DJI_SHOOT_GROUP].motor_data[DJI_MOTOR_SHOOT_FEED_M2006_RX_3].speed;
}


void Shoot_Control(void)
{
    switch(Robot.status.fire_mode){
        case SHOOT_STOP:
            Shoot.friction_motor[0].target_speed = 0;
            Shoot.friction_motor[1].target_speed = 0;
            Shoot.dial_motor.target_speed = 0;
            break;
        case SHOOT_FIRE:
            Shoot.friction_motor[0].target_speed = -6000 * Dr16.data.RC_Value.Wheel;	// 根据遥控器拨轮的值调整射速
            Shoot.friction_motor[1].target_speed = 6000 * Dr16.data.RC_Value.Wheel;	
            Shoot.dial_motor.target_speed = 6000;
            break;
        case SHOOT_BACK:
            Shoot.friction_motor[0].target_speed = 0;
            Shoot.friction_motor[1].target_speed = 0;
            Shoot.dial_motor.target_speed = -1000;
            break;
        default:
            break;
    }

    int16_t right_speed_out = Pid_Calc(&Shoot.friction_motor[0].speed_pid, Shoot.friction_motor[0].target_speed, Shoot.friction_motor[0].current_speed);
    Shoot.friction_motor[0].speed_pid.pid_data.output = right_speed_out;

    int16_t left_speed_out = Pid_Calc(&Shoot.friction_motor[1].speed_pid, Shoot.friction_motor[1].target_speed, Shoot.friction_motor[1].current_speed);
    Shoot.friction_motor[1].speed_pid.pid_data.output = left_speed_out;

    int16_t dial_speed_out = Pid_Calc(&Shoot.dial_motor.speed_pid, Shoot.dial_motor.target_speed, Shoot.dial_motor.current_speed);
    Shoot.dial_motor.speed_pid.pid_data.output = dial_speed_out;

    Dji_Motor_Set_Current(DJI_SHOOT_GROUP, 
						  DJI_MOTOR_TX_1_4,  
                          Shoot.friction_motor[0].speed_pid.pid_data.output, 
                          Shoot.friction_motor[1].speed_pid.pid_data.output,
                          Shoot.dial_motor.speed_pid.pid_data.output,
                          0);
}


void Shoot_Task(void)
{
	if(!Shoot.flag){		// 使能判断
		return;
	}
	
	if(Shoot.tick < SHOOT_TICK_TIME){		// 定时判断
		return;
	}
	
	Shoot.tick = 0;		// 清除定时周期
    
	Shoot_Friction_Get_Data();
    Shoot_Dial_Get_Data();

    Shoot_Control();
}


void Shoot_Timing_Callback(void)
{
	if(Shoot.tick < SHOOT_TICK_TIME){
		Shoot.tick++;
	}
}

