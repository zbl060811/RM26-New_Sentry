#include "public_cmd.h"
#include "robot.h"
#include "at9s.h"
#include "dr16.h"
#include "hi14.h"
#include "config.h"



PublicCmdTypeDef Public = {
    {&hcan2, LOWER_BORAD_CAN_ID, LOCAL_CAN_ID},
    {&hcan2, DISTRIBUTION_CAN_ID, LOCAL_CAN_ID}
};


void Public_Cmd_Init(void)
{
    Public.flag = 1;
    Public.tick = 0;
}


void Public_Send_Rc(void)
{
    Public.rc_data.cmd_type = CAN_MSG_RC;

    #if CONFIG_USE_REMOTE
        Public.rc_data.rc_right_x = (int16_t)(Dr16.data.RC_Value.CH0 * 1000);
        Public.rc_data.rc_right_y = (int16_t)(Dr16.data.RC_Value.CH1 * 1000);
        Public.rc_data.rc_left_x = (int16_t)(Dr16.data.RC_Value.CH2 * 1000);
    #else
        Public.rc_data.rc_right_x = (int16_t)(At9s.at9s_rc.right_x * 1000);
        Public.rc_data.rc_right_y = (int16_t)(At9s.at9s_rc.right_y * 1000);
        Public.rc_data.rc_left_x = (int16_t)(At9s.at9s_rc.left_x * 1000);
    #endif
    

    Communicate_Send(&Public.up_low_com, (uint8_t *)&Public.rc_data, 8);
}


void Public_Send_Eular(void)
{
    Public.eular_data.cmd_type = CAN_MSG_EULAR;
    Public.eular_data.yaw_rate = (int16_t)(Hi14.hi_data.gyro.gz * 100);
    Public.eular_data.pitch = (int16_t)(Hi14.hi_data.euler.pitch * 100);
    Public.eular_data.yaw = (int16_t)(Hi14.hi_data.euler.yaw * 100);

    Communicate_Send(&Public.up_low_com, (uint8_t *)&Public.eular_data, 8);
}


void Public_Send_Status(void)
{
    Public.status_data.cmd_type = CAN_MSG_STATUS;
    Public.status_data.rc_status = Dr16.status;
    Public.status_data.sport_mode = Robot.status.sport_mode;

    Communicate_Send(&Public.up_low_com, (uint8_t *)&Public.status_data, 8);
}


void Public_Cmd_Task(void)
{
    if(!Public.flag || Public.tick < PUBLIC_CMD_TICK_TIME){
        return;
    }
    Public.tick = 0;
    
    Public_Send_Rc();
    Public_Send_Eular();
    Public_Send_Status();
}


void Public_Cmd_Timing_Callback(void) 
{
    if(Public.tick < PUBLIC_CMD_TICK_TIME){
        Public.tick++;
    }
}




