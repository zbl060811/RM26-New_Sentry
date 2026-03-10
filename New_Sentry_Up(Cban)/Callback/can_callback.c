#include "can_callback.h"
#include "dji_motor.h"
#include "gimbal.h"




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
		Dji_Motor_Set_Current(DJI_GIMBAL_GROUP, DJI_MOTOR_TX_1_4, Gimbal.yaw_motor.pid_speed.pid_data.output, 
                                                              0, 
                                                              0,
                                                              0);   
	}	
	
	if(hcan == &hcan2)
	{
		error = HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, Bsp_Can2_Fifo0_Rx_Data);
		if(error != HAL_OK){
			Error_Handler();		// hardware error encountered
		}
		
		Dji_Motor_Get_Data(DJI_SHOOT_GROUP, &rx_header, Bsp_Can2_Fifo0_Rx_Data);
	}
}


