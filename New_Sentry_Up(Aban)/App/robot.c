#include "robot.h"
#include "shoot.h"
#include "config.h"

#include "at9s.h"
#include "dr16.h"


RobotTypeDef Robot;



void Robot_Init(void)
{
    Robot.flag = 1;
    Robot.tick = 0;


}


void Robot_Control(void)
{
    // 运动模式
    if(Dr16.data.RC_Value.S2 == 1)   Robot.status.sport_mode = FOLLOW_GIMBAL;
    else if(Dr16.data.RC_Value.S2 == 3)  Robot.status.sport_mode = GYROSCOPE;
    

    // 控制方式
    if(Dr16.data.RC_Value.S1 == 1)  Robot.status.control_mode = FIRE_MODE_MANUAL;
    else if(Dr16.data.RC_Value.S1 == 3) Robot.status.control_mode = FIRE_MODE_DEBUG;
    else Robot.status.control_mode = FIRE_MODE_AUTO;


    // 战斗模式
    if(Dr16.data.RC_Value.Wheel == 0)   Robot.status.fire_mode = SHOOT_STOP;
    if(Dr16.data.RC_Value.Wheel >= 0.1f) Robot.status.fire_mode = SHOOT_FIRE;
    if(Dr16.data.RC_Value.Wheel <= -0.1f) Robot.status.fire_mode = SHOOT_BACK;
}


void Robot_Task(void)
{
    if(!Robot.flag || Robot.tick < ROBOT_TICK_TIME){ 
        return;
    }

    Robot.tick = 0;

    Robot_Control();
}


void Robot_Timing_Callback(void)
{
    if(Robot.tick < ROBOT_TICK_TIME){
        Robot.tick++;
    }
}

