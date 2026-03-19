#include "communicate.h"
#include "referee.h"



CanCommunicateTypeDef Can_Communicate = {
    &hcan2,
    CAN_COMMUNICATE_LOCAL_ID,
    CAN_COMMUNICATE_SEND_ID,
    {0, 0}
};


void Communicate_Init(void)
{
    Can_Communicate.data.flag = 1;
    Can_Communicate.data.tick = 0;

    Communicate_Rx_Filter_Set();
}


void Communicate_Rx_Filter_Set(void)
{
    BspCanRxFilterTypeDef can_rx_filter;
	
	can_rx_filter.filter_id.sub.STID = 0x0000;
	can_rx_filter.filter_id.sub.EXID = 0x0000;
	can_rx_filter.filter_id.sub.IDE = 0;
	can_rx_filter.filter_id.sub.RTR = 0;
	
	can_rx_filter.filter_mask_id.sub.STID = 0x0000;
	can_rx_filter.filter_mask_id.sub.EXID = 0x0000;
	can_rx_filter.filter_mask_id.sub.IDE = 1;
	can_rx_filter.filter_mask_id.sub.RTR = 1;
	
	can_rx_filter.filter_bank = 14;				// use filter group 0
	can_rx_filter.hcan = Can_Communicate.hcan;				// use hcan2
	can_rx_filter.fifox = CAN_FilterFIFO0;		// use filter FIFO0
	can_rx_filter.filter_activation = CAN_FILTER_ENABLE;		// activate can filter
	
	Bsp_Can_Rx_Filter_Set(&can_rx_filter);
}


char Communicate_Get_Data(CanCommunicateTypeDef *can, CAN_RxHeaderTypeDef *rxheader, uint8_t *rx_buffer)
{
    uint16_t rx_stdid = 0;
    uint16_t rx_num = 0;

    rx_stdid = rxheader->StdId;
    rx_num = rxheader->DLC;

    // 判断接收到的ID是否为服务ID，且数据长度正确
    if(rx_stdid != can->TX_STD_ID || rx_num != 8){
        return ERROR;
    }

    Communicate_Data_Process(rx_buffer);
    return SUCCESS;
}


HAL_StatusTypeDef Communicate_Send(CanCommunicateTypeDef *can, uint8_t *data, uint8_t len)
{
    CAN_TxHeaderTypeDef tx_header;
	uint32_t tx_email = CAN_TX_MAILBOX0;
 
	tx_header.StdId = can->TX_STD_ID;
	tx_header.IDE = CAN_ID_STD;
	tx_header.RTR = CAN_RTR_DATA;
	tx_header.DLC = len;
	tx_header.TransmitGlobalTime = DISABLE;

	return HAL_CAN_AddTxMessage(can->hcan, &tx_header, data, &tx_email);
}


void Communicate_Data_Process(uint8_t *rx_buffer)
{   
    int16_t yaw_rate, yaw;
    int16_t vision_yaw, vision_pitch;
    
    switch(rx_buffer[0]){
        case CAN_MSG_RC_LEFT:
            Can_Communicate.data.rc_left_x = (int16_t)(rx_buffer[2] << 8 | rx_buffer[1]);
            Can_Communicate.data.rc_left_y = (int16_t)(rx_buffer[4] << 8 | rx_buffer[3]);
            Can_Communicate.data.rc_status = rx_buffer[5];   // 处理机器人状态数据
            Can_Communicate.data.sport_mode = rx_buffer[6];  // 运动模式
            Can_Communicate.data.ctrl_mode = rx_buffer[7];   // 控制模式

            Can_Communicate.data.rc_left_x = Can_Communicate.data.rc_left_x / 1000.0f;
            Can_Communicate.data.rc_left_y = Can_Communicate.data.rc_left_y / 1000.0f;        
        break;

        case CAN_MSG_RC_RIGHT:
            Can_Communicate.data.rc_right_x = (int16_t)(rx_buffer[2] << 8 | rx_buffer[1]);
            Can_Communicate.data.rc_right_y = (int16_t)(rx_buffer[4] << 8 | rx_buffer[3]);
            yaw_rate = (int16_t)(rx_buffer[6] << 8 | rx_buffer[5]);
            Can_Communicate.data.target_found = rx_buffer[7];   // 是否检测到目标

            Can_Communicate.data.yaw_rate = yaw_rate / 100.0f;
            Can_Communicate.data.rc_right_x = Can_Communicate.data.rc_right_x / 1000.0f;
            Can_Communicate.data.rc_right_y = Can_Communicate.data.rc_right_y / 1000.0f;
        break;

        case CAN_MSG_VISION:
            Can_Communicate.data.target_id = rx_buffer[1];      // 目标ID
            vision_yaw = (int16_t)(rx_buffer[3] << 8 | rx_buffer[2]);
            vision_pitch = (int16_t)(rx_buffer[5] << 8 | rx_buffer[4]);
            yaw = (int16_t)(rx_buffer[7] << 8 | rx_buffer[6]);

            Can_Communicate.data.vision_yaw = vision_yaw / 1000.0f;
            Can_Communicate.data.vision_pitch = vision_pitch / 1000.0f;
            Can_Communicate.data.yaw = yaw / 100.0f;
            break;

        default:
            break;
    }
}

void Communicate_Send_Data(void)
{
    uint8_t tx_buffer[8] = {0};

    tx_buffer[0] = CAN_MSG_HEAT;
    tx_buffer[1] = referee_info.PowerHeatData.shooter_17mm_1_barrel_heat << 8;
    tx_buffer[2] = referee_info.PowerHeatData.shooter_17mm_1_barrel_heat;

    Communicate_Send(&Can_Communicate, tx_buffer, CAN_CMD_MAX_DATA_LEN);
}

void Communicate_Task(void)
{
    if(!Can_Communicate.data.flag || Can_Communicate.data.tick < CAN_COMMUNICATE_TICK_TIME){ 
		return;
	}
    Can_Communicate.data.tick = 0;

    Communicate_Send_Data();
}


void Communicate_Timing_Callback(void)
{
    if(Can_Communicate.data.tick < CAN_COMMUNICATE_TICK_TIME){
        Can_Communicate.data.tick++;
    }
}

