#include "comm.h"




void Communicate_Init(void)
{
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
	can_rx_filter.hcan = &hcan2;				// use hcan2
	can_rx_filter.fifox = CAN_FilterFIFO0;		// use filter FIFO0
	can_rx_filter.filter_activation = CAN_FILTER_ENABLE;		// activate can filter
	
	Bsp_Can_Rx_Filter_Set(&can_rx_filter);
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


char Communicate_Get_Data(CanCommunicateTypeDef *can, CAN_RxHeaderTypeDef *rxheader, uint8_t *rx_buffer)
{
    uint16_t rx_stdid = 0;
    uint16_t rx_num = 0;

    rx_stdid = rxheader->StdId;
    rx_num = rxheader->DLC;

    // 判断接收到的ID是本机ID，且数据长度正确
    if(rx_stdid != can->RX_STD_ID || rx_num != 8){
        return ERROR;
    }

    Communicate_Data_Process(rx_buffer);
    return SUCCESS;
}


void Communicate_Data_Process(uint8_t *rx_buffer)
{   
    switch(rx_buffer[0]){
        case 0x01:
            
            break;
        
        case 0x02:
            
            break;

        case 0x03:
            
            break;

        default:
            break;
    }
}

