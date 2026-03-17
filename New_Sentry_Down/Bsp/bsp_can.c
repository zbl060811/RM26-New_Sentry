#include "bsp_can.h"



uint8_t Bsp_Can1_Fifo0_Rx_Data[FIFO_RX_DATA];
uint8_t Bsp_Can1_Fifo1_Rx_Data[FIFO_RX_DATA];
uint8_t Bsp_Can2_Fifo0_Rx_Data[FIFO_RX_DATA];
uint8_t Bsp_Can2_Fifo1_Rx_Data[FIFO_RX_DATA];




/* 
 * @brief	bsp_can init
 * @para	none
 * @return	void
*/
void Bsp_Can_Init(void)
{
	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
	HAL_CAN_Start(&hcan2);
	HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
}


/* 
 * @brief	bsp_can receive filter set
 * @para	none
 * @return	void
*/
void Bsp_Can_Rx_Filter_Set(BspCanRxFilterTypeDef *rx_filter_para)
{
	CAN_FilterTypeDef can_filter_config;
	
	can_filter_config.FilterBank = rx_filter_para->filter_bank;		// filter group number
	can_filter_config.FilterMode = CAN_FILTERMODE_IDMASK;			// id block mode
																	// filter range 0x200~0x20F
	
	can_filter_config.FilterScale = CAN_FILTERSCALE_32BIT;			// 32bit filter
	
	
	can_filter_config.FilterIdHigh = rx_filter_para->filter_id.value.HIGH;
	can_filter_config.FilterIdLow = rx_filter_para->filter_id.value.LOW;
	
	can_filter_config.FilterMaskIdHigh = rx_filter_para->filter_mask_id.value.HIGH;
	can_filter_config.FilterMaskIdLow = rx_filter_para->filter_mask_id.value.LOW;
	
	can_filter_config.FilterFIFOAssignment = rx_filter_para->fifox;		// choice can receive fifo
	can_filter_config.SlaveStartFilterBank = rx_filter_para->slave_start_filter_bank;	// can1(0-13)��can2(14-27)Each gets half filter
	can_filter_config.FilterActivation = rx_filter_para->filter_activation;
	
	HAL_CAN_ConfigFilter(rx_filter_para->hcan, &can_filter_config);		// config can filter
}


