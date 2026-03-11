#ifndef __COMM_H__
#define __COMM_H__ 



#include "bsp_can.h"




// 本机ID：0X102     服务ID：0X100
#define CAN_COMMUNICATE_LOCAL_ID      0X102        
#define CAN_COMMUNICATE_SEND_ID       0X100

#define CAN_COMMUNICATE_USE_CAN      hcan2


#pragma pack(1)




typedef struct Can_Communicate_Struct_T
{
    CAN_HandleTypeDef *hcan;
    uint16_t  TX_STD_ID;
    uint16_t  RX_STD_ID;
} CanCommunicateTypeDef;



#pragma pack()



void Communicate_Init(void);
void Communicate_Rx_Filter_Set(void);
HAL_StatusTypeDef Communicate_Send(CanCommunicateTypeDef *can, uint8_t *data, uint8_t len);
void Communicate_Data_Process(uint8_t *rx_buffer);
char Communicate_Get_Data(CanCommunicateTypeDef *can, CAN_RxHeaderTypeDef *rxheader, uint8_t *rx_buffer);




#endif
