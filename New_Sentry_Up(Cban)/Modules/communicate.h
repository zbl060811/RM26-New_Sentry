#ifndef __COMMUNICATE_H__
#define __COMMUNICATE_H__


#include "rm_typedef.h"
#include "bsp_can.h"


#define CAN_COMMUNICATE_TICK_TIME   10


/*
    本机ID：0X100     
    下板ID：0X101
    分线板ID：0X102
*/
#define LOCAL_CAN_ID             0X100        
#define LOWER_BORAD_CAN_ID       0X101
#define DISTRIBUTION_CAN_ID      0X102



#pragma pack(1)




typedef struct Can_Communicate_Struct_T
{
    CAN_HandleTypeDef *hcan;
    uint16_t  TX_STD_ID;
    uint16_t  RX_STD_ID;
} CanCommunicateTypeDef;




#pragma pack()





/* *************** Communicate Function *************** */
void Communicate_Init(void);
void Communicate_Rx_Filter_Set(void);
HAL_StatusTypeDef Communicate_Send(CanCommunicateTypeDef *can, uint8_t *data, uint8_t len);
char Communicate_Get_Data(CanCommunicateTypeDef *can, CAN_RxHeaderTypeDef *rxheader, uint8_t *rx_buffer);
void Communicate_Data_Process(uint8_t *rx_buffer);



#endif

