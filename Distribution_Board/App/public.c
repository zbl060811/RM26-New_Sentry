#include "public.h"



PublicCmdTypeDef Public;



void Public_Cmd_Init(void)
{
    Public.flag = 1;
    Public.tick = 0;

    Public.can_com.hcan = &CAN_COMMUNICATE_USE_CAN;
    Public.can_com.TX_STD_ID = CAN_COMMUNICATE_SEND_ID;
    Public.can_com.RX_STD_ID = CAN_COMMUNICATE_LOCAL_ID;
}


void Public_Send_NX(void)
{
    Public.navigation_data.cmd_type = CAN_MSG_NX_NAVIGATION;
    Public.navigation_data.x = 12;
    Public.navigation_data.y = 34;
    Public.navigation_data.z = 56;
    

    Communicate_Send(&Public.can_com, (uint8_t *)&Public.navigation_data, 8);
}



void Public_Cmd_Task(void)
{
    if(!Public.flag || Public.tick < PUBLIC_CMD_TICK_TIME){
        return;
    }
    Public.tick = 0;
    
	Public_Send_NX();
}


void Public_Cmd_Timing_Callback(void) 
{
    if(Public.tick < PUBLIC_CMD_TICK_TIME){
        Public.tick++;
    }
}

