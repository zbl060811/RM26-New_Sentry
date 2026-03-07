#include "dji_motor.h"
#include "rm_typedef.h"


DjiMotorDataTypeDef Dji_Chassis_Group_Data[DJI_MOTOR_CHASSIS_RX_SUM];
DjiMotorDataTypeDef Dji_Chassis_Yaw_Group_Data[DJI_MOTOR_CHASSIS_YAW_RX_SUM];



DjiMotorGroupTypeDef DJI_Motor[DJI_GROUP_SUM] = {
	// 从左到右成员顺序为:hcan--TX_STD_ID--RX_STD_ID--Data--SUM
	{&hcan1, {0x200, 0x1FF}, 0x200, Dji_Chassis_Group_Data, DJI_MOTOR_CHASSIS_RX_SUM},
	{&hcan1, {0x1FF, 0X2FF}, 0X204, Dji_Chassis_Yaw_Group_Data, DJI_MOTOR_CHASSIS_YAW_RX_SUM},
};


/* 
 * @brief	dji_motor init
 * @para	none
 * @return	void
 */
void Dji_Motor_Init(void)
{
	Dji_Motor_Chassis_Rx_Filter_Set();  
}


/* 
 * @brief	dji motor receive filter set
 * @para	none
 * @return	void
*/
void Dji_Motor_Chassis_Rx_Filter_Set(void)
{
	BspCanRxFilterTypeDef can_rx_filter;
	
	can_rx_filter.filter_id.sub.STID = 0x200;
	can_rx_filter.filter_id.sub.EXID = 0x0000;
	can_rx_filter.filter_id.sub.IDE = 0;
	can_rx_filter.filter_id.sub.RTR = 0;
	
	can_rx_filter.filter_mask_id.sub.STID = 0x7E0;
	can_rx_filter.filter_mask_id.sub.EXID = 0x0000;
	can_rx_filter.filter_mask_id.sub.IDE = 1;
	can_rx_filter.filter_mask_id.sub.RTR = 1;
	
	can_rx_filter.filter_bank = 0;				// use filter group 0
	can_rx_filter.hcan = &hcan1;				// use hcan1
	can_rx_filter.fifox = CAN_FilterFIFO0;		// use filter FIFO0
	can_rx_filter.filter_activation = CAN_FILTER_ENABLE;		// activate can filter
	
	Bsp_Can_Rx_Filter_Set(&can_rx_filter);
}


char Dji_Motor_Get_Data(DjiMotorGroupEnum dji_group_x, CAN_RxHeaderTypeDef *rxheader, uint8_t *motor_rx_buffer)
{
	uint16_t rx_stdid = 0;
	uint16_t rx_num = 0;
	uint8_t dji_rx_sum = 0;
	
	
	rx_stdid = rxheader->StdId;
	dji_rx_sum = DJI_Motor[dji_group_x].data_length;
	rx_num = rx_stdid - (DJI_Motor[dji_group_x].RX_STD_ID + 1);
	
	// 判断接收到的ID是否小于当前组的最大电机数，小于才能通过否则return
	if((rx_num + 1) > dji_rx_sum){
		return ERROR;
	}
	
	DJI_Motor[dji_group_x].motor_data[rx_num].rx_flag = 1;
	Dji_Motor_Data_Process(motor_rx_buffer, dji_group_x, rx_num);
	
	return SUCCESS;
}


void Dji_Motor_Data_Process(uint8_t *motor_rx_buff, DjiMotorGroupEnum dji_group_x, uint8_t motor_id)
{
	uint16_t angle = 0;
	int16_t speed = 0;
	int16_t current = 0;
	
	angle = ((uint16_t)motor_rx_buff[0] << 8) | (uint16_t)(motor_rx_buff[1]);
    speed = ((uint16_t)motor_rx_buff[2] << 8) | (uint16_t)(motor_rx_buff[3]);
    current = ((uint16_t)motor_rx_buff[4] << 8) | (uint16_t)(motor_rx_buff[5]);
    DJI_Motor[dji_group_x].motor_data[motor_id].temperature = motor_rx_buff[6];
	
	DJI_Motor[dji_group_x].motor_data[motor_id].angle = angle;     		  // 角度不归一化
    DJI_Motor[dji_group_x].motor_data[motor_id].speed = speed;            // 速度不进行归一化
    DJI_Motor[dji_group_x].motor_data[motor_id].current = (float)current / 16384; // 电流归一化
}


void Dji_Motor_Set_Current(DjiMotorGroupEnum dji_group_x, DjiMotorTxIdEnum tx_id, int motor1, int motor2, int motor3, int motor4)
{
	CAN_TxHeaderTypeDef tx_header;
	uint32_t tx_email = CAN_TX_MAILBOX0;
	uint8_t can_tx_data[8];
	
	tx_header.StdId = DJI_Motor[dji_group_x].TX_STD_ID[tx_id];
	tx_header.IDE = CAN_ID_STD;
	tx_header.RTR = CAN_RTR_DATA;
	tx_header.DLC = 0x08;
	
	can_tx_data[0] = (uint8_t)(motor1 >> 8);
	can_tx_data[1] = (uint8_t)(motor1);
	can_tx_data[2] = (uint8_t)(motor2 >> 8);
	can_tx_data[3] = (uint8_t)(motor2);
	can_tx_data[4] = (uint8_t)(motor3 >> 8);
	can_tx_data[5] = (uint8_t)(motor3);
	can_tx_data[6] = (uint8_t)(motor4 >> 8);
	can_tx_data[7] = (uint8_t)(motor4);
	
	HAL_CAN_AddTxMessage(DJI_Motor[dji_group_x].hcan, &tx_header, can_tx_data, &tx_email);
}



