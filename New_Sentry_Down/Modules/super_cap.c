#include "super_cap.h"
#include "log.h"



SuperCapTypeDef Super_Cap = {
	&hcan2, 
	0x210, 
	0x211, 
	{0,0,0,0}
};



void Super_Cap_Get_Data( CAN_RxHeaderTypeDef *rxheader, uint8_t *cap_rx_buff)
{
	uint16_t i_vol = 0;
	uint16_t c_vol = 0;
	uint16_t i_current = 0;
	uint16_t t_power = 0;
	
	if(rxheader->StdId == 0x211)
	{
		i_vol = ((uint16_t)cap_rx_buff[0] | (uint16_t)(cap_rx_buff[1] << 8));
		c_vol = ((uint16_t)cap_rx_buff[2] | (uint16_t)(cap_rx_buff[3] << 8));
		i_current = ((uint16_t)cap_rx_buff[4] | (uint16_t)(cap_rx_buff[5] << 8));
		t_power = ((uint16_t)cap_rx_buff[6] | (uint16_t)(cap_rx_buff[7] << 8));
		
		Super_Cap.cap_data.input_vol = i_vol / 100.f;
		Super_Cap.cap_data.cap_vol = c_vol / 100.f;
		Super_Cap.cap_data.input_current = i_current / 100.f;
		Super_Cap.cap_data.target_power = t_power / 100.f;
	}	
}


uint8_t Super_Cap_Set_Power(float target_power)
{
	CAN_TxHeaderTypeDef tx_header;
	uint32_t tx_email = CAN_TX_MAILBOX0;
	uint8_t send_buf[2] = {0};
	
	if(target_power < 30 || target_power > 130){
//		LOG_SUPER_CAP("super cap target power beyond reasonable limits");
		return ERROR;
	}
	
	uint16_t power_value = (uint16_t)(target_power * 100.0f);
	
	send_buf[0] = (power_value >> 8) & 0xFF;
	send_buf[1] = power_value & 0xFF;
	
	tx_header.StdId = Super_Cap.TX_STD_ID;
	tx_header.DLC = 8;
	tx_header.IDE = CAN_ID_STD;
	tx_header.RTR = CAN_RTR_DATA;
	
	return HAL_CAN_AddTxMessage(Super_Cap.hcan, &tx_header, send_buf, &tx_email);
}




	


