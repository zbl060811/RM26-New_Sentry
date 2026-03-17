#include "public_cmd.h"
#include "robot.h"
#include "at9s.h"
#include "dr16.h"
#include "hi14.h"
#include "nx_topic.h"
#include "config.h"



PublicCmdTypeDef Public = {
    {&hcan2,
    CAN_COMMUNICATE_SEND_ID,
    CAN_COMMUNICATE_LOCAL_ID},
};


void Public_Cmd_Init(void)
{
    Public.flag = 1;
    Public.tick = 0;
}


void Public_Send_RcLeft(void)
{
    Public.rc_left_data.cmd_type = CAN_MSG_RC_LEFT;

    Public.rc_left_data.rc_left_x = (int16_t)(Dr16.data.RC_Value.CH2 * 1000);
    Public.rc_left_data.rc_left_y = (int16_t)(Dr16.data.RC_Value.CH3 * 1000);
    Public.rc_left_data.rc_status = Dr16.status;
    Public.rc_left_data.sport_mode = Robot.status.sport_mode;
    Public.rc_left_data.ctrl_mode = Robot.status.control_mode;    

    Communicate_Send(&Public.can_com, (uint8_t *)&Public.rc_left_data, PUBLIC_CMD_MAX_DATA_LEN);
}


void Public_Send_RcRight(void)
{
    Public.rc_right_data.cmd_type = CAN_MSG_RC_RIGHT;

    Public.rc_right_data.rc_right_x = (int16_t)(Dr16.data.RC_Value.CH0 * 1000);
    Public.rc_right_data.rc_right_y = (int16_t)(Dr16.data.RC_Value.CH1 * 1000);
    Public.rc_right_data.yaw_rate = (int16_t)(Hi14.hi_data.gyro.gz * 100);
    Public.rc_right_data.target_found = vision_topic.parsed_frame.data.combined.target_found;

    Communicate_Send(&Public.can_com, (uint8_t *)&Public.rc_right_data, PUBLIC_CMD_MAX_DATA_LEN);
}

void Public_Send_Vision(void)
{
    Public.vision_data.cmd_type = CAN_MSG_VISION;
    Public.vision_data.target_id = vision_topic.parsed_frame.data.combined.target_id;
    Public.vision_data.vision_yaw = (int16_t)(vision_topic.parsed_frame.data.combined.yaw * 1000);
    Public.vision_data.vision_pitch = (int16_t)(vision_topic.parsed_frame.data.combined.pitch * 1000);
    Public.vision_data.yaw = (int16_t)(Hi14.hi_data.euler.yaw * 100);

    Communicate_Send(&Public.can_com, (uint8_t *)&Public.vision_data, PUBLIC_CMD_MAX_DATA_LEN);
}

void Public_Cmd_Task(void)
{
    if(!Public.flag || Public.tick < PUBLIC_CMD_TICK_TIME){
        return;
    }
    Public.tick = 0;
    
    Public_Send_RcLeft();
    Public_Send_RcRight();
    Public_Send_Vision();
}


void Public_Cmd_Timing_Callback(void) 
{
    if(Public.tick < PUBLIC_CMD_TICK_TIME){
        Public.tick++;
    }
}




