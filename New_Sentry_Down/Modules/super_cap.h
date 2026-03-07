#ifndef __SUPER_CAP_H__
#define __SUPER_CAP_H__


#include "bsp_can.h"




#pragma pack(1)

typedef struct Super_Cap_Data_Struct_T
{
	float input_vol;	// 输入电压
	float cap_vol;		// 电容电压
	float input_current;	// 输入电流
	float target_power;		// 目标功率
} SuperCapDataTypeDef;


typedef struct Super_Cap_Struct_T
{
	CAN_HandleTypeDef *hcan;
	
	uint16_t TX_STD_ID;
	uint16_t RX_STD_ID;
	
	SuperCapDataTypeDef cap_data;
} SuperCapTypeDef;

#pragma pack()



/* *************** Super Cap Function *************** */
void Super_Cap_Get_Data( CAN_RxHeaderTypeDef *rxheader, uint8_t *cap_rx_buff);
uint8_t Super_Cap_Set_Power(float target_power);






#endif
