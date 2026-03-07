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
    // 发射模式
    #if CONFIG_USE_REMOTE
        if(Dr16.data.RC_Value.S1 == 1)   Shoot.mode = SHOOT_FIRE;
        else if(Dr16.data.RC_Value.S1 == 3)  Shoot.mode = SHOOT_STOP;
        else if(Dr16.data.RC_Value.S1 == 2)  Shoot.mode = SHOOT_BACK;
    #else
        if(At9s.at9s_rc.grade_c == 1)   Shoot.mode = SHOOT_FIRE;
        else if(At9s.at9s_rc.grade_c == 3)  Shoot.mode = SHOOT_STOP;
        else if(At9s.at9s_rc.grade_c == 2)  Shoot.mode = SHOOT_BACK;
    #endif

    

    // 运动模式
    #if CONFIG_USE_REMOTE
        if(Dr16.data.RC_Value.S2 == 1)   Robot.status.sport_mode = FOLLOW_GIMBAL;
        else if(Dr16.data.RC_Value.S2 == 3)  Robot.status.sport_mode = GYROSCOPE;
    #else
        if(At9s.at9s_rc.grade_b == 1)   Robot.status.sport_mode = FOLLOW_GIMBAL;
        else if(At9s.at9s_rc.grade_b == 2)  Robot.status.sport_mode = GYROSCOPE;
    #endif

    

    // 控制方式
    


    // 战斗模式

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

