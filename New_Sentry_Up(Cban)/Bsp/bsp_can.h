#ifndef __BSP_CAN_H
#define __BSP_CAN_H


#include "can.h"



#define FIFO_RX_DATA		8		// FIFO receive data size


#pragma pack(1)		// Align the specified structure according to 1-byte alignment

typedef union
{
	struct{
		uint16_t LOW;		// low position
		uint16_t HIGH;		// high position
	} value;
	
	struct{
		uint8_t REV : 1;	// [0] : none
		uint8_t RTR : 1;	// [1] : 0:data frame/1:remote frame
		uint8_t IDE : 1;	// [2] : 0:STID/1:EXID
		uint32_t EXID : 18;		// [21:3] : store EXID
		uint32_t STID : 11;		// [31:22] : store STID
	} sub;
} BspCanRxFilterIdTypeDef;

typedef struct _Bsp_Can_Rx_Filter_Struct
{
	BspCanRxFilterIdTypeDef filter_id;			// need to filter id
	BspCanRxFilterIdTypeDef filter_mask_id;		// mask of code
	
	uint8_t filter_bank;				// filter group number
	uint8_t slave_start_filter_bank;	// initial filter group
	CAN_HandleTypeDef *hcan;			// CAN handle
	uint32_t fifox;						// FIFO choice
	uint32_t filter_activation;			// activate filter
} BspCanRxFilterTypeDef;

#pragma pack()		// Disable structure alignment



extern uint8_t Bsp_Can1_Fifo0_Rx_Data[FIFO_RX_DATA];
extern uint8_t Bsp_Can1_Fifo1_Rx_Data[FIFO_RX_DATA];
extern uint8_t Bsp_Can2_Fifo0_Rx_Data[FIFO_RX_DATA];
extern uint8_t Bsp_Can2_Fifo1_Rx_Data[FIFO_RX_DATA];


/* *************** Bsp_can Function *************** */
void Bsp_Can_Init(void);
void Bsp_Can_Rx_Filter_Set(BspCanRxFilterTypeDef *rx_filter_para);




#endif

