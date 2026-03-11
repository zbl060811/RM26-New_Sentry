#include "can_callback.h"

#include "public.h"

#include "comm.h"



void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	uint8_t error = 0;
	CAN_RxHeaderTypeDef rx_header;
	
	if(hcan == &hcan2)
	{
		error = HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, Bsp_Can2_Fifo0_Rx_Data);
		if(error != HAL_OK){
			Error_Handler();		// hardware error encountered
		}
		
		Communicate_Get_Data(&Public.can_com, &rx_header, Bsp_Can2_Fifo0_Rx_Data);
	}
}
