#include "can_callback.h"
#include "dji_motor.h"
#include "public_cmd.h"




void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	uint8_t error = 0;
	CAN_RxHeaderTypeDef rx_header;
	
	if(hcan == &hcan1)
	{
		error = HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, Bsp_Can1_Fifo0_Rx_Data);
		if(error != HAL_OK){
			Error_Handler();		// hardware error encountered
		}
			
		Dji_Motor_Get_Data(DJI_GIMBAL_GROUP, &rx_header, Bsp_Can1_Fifo0_Rx_Data);
	}	
	
	if(hcan == &hcan2)
	{
		error = HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, Bsp_Can2_Fifo0_Rx_Data);
		if(error != HAL_OK){
			Error_Handler();		// hardware error encountered
		}
		
		Dji_Motor_Get_Data(DJI_SHOOT_GROUP, &rx_header, Bsp_Can2_Fifo0_Rx_Data);
		Communicate_Get_Data(&Public.can_com, &rx_header, Bsp_Can2_Fifo0_Rx_Data);
	}
}


