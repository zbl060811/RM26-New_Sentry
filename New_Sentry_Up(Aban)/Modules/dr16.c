#include "dr16.h"
#include "usart.h"


Dr16TypeDef Dr16 = {0};



/// @brief 遥控器接收初始化
/// @param  无
void DJI_DR16_Init(Dr16TypeDef *dr16)
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, dr16->buffer, sizeof(dr16->buffer)); // 串口空闲中断接收DMA
    __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);                                // 关闭DMA接收一半中断
}


uint8_t *DJI_DR16_Get_Buffer(Dr16TypeDef *dr16)
{
    return dr16->buffer;
}


void DJI_DR16_Data_Process(Dr16TypeDef *dr16, uint8_t *buffer, uint8_t size)
{
    if (buffer == NULL || size != DJI_DR16_DATA_LENGHT)
    {
        return;
	}

	dr16->data.RC.CH0 = ((int16_t)buffer[0] | ((int16_t)buffer[1] << 8)) & 0x07FF;
	dr16->data.RC.CH1 = (((int16_t)buffer[1] >> 3) | ((int16_t)buffer[2] << 5)) & 0x07FF;
	dr16->data.RC.CH2 = (((int16_t)buffer[2] >> 6) | ((int16_t)buffer[3] << 2) | ((int16_t)buffer[4] << 10)) & 0x07FF;
	dr16->data.RC.CH3 = (((int16_t)buffer[4] >> 1) | ((int16_t)buffer[5] << 7)) & 0x07FF;
	dr16->data.RC.Wheel = (((int16_t)buffer[16]) | ((int16_t)buffer[17] << 8));
	dr16->data.RC.S1 = ((buffer[5] >> 4) & 0x000C) >> 2;
	dr16->data.RC.S2 = ((buffer[5] >> 4) & 0x0003);
	
	dr16->data.RC_Value.CH0 = NORMALIZE(dr16->data.RC.CH0, 364, 1684);			 // 数据归一化为（-1~1）
    dr16->data.RC_Value.CH1 = NORMALIZE(dr16->data.RC.CH1, 364, 1684);             // 数据归一化为（-1~1）
    dr16->data.RC_Value.CH2 = NORMALIZE(dr16->data.RC.CH2, 364, 1684);             // 数据归一化为（-1~1）
    dr16->data.RC_Value.CH3 = NORMALIZE(dr16->data.RC.CH3, 364, 1684);             // 数据归一化为（-1~1）
    dr16->data.RC_Value.Wheel = NORMALIZE(dr16->data.RC.Wheel, 364, 1684);         // 数据归一化为（-1~1）
    dr16->data.RC_Value.S1 = dr16->data.RC.S1;
    dr16->data.RC_Value.S2 = dr16->data.RC.S2;
	
	dr16->data.Mouse.X = ((int16_t)buffer[6]) | ((int16_t)buffer[7] << 8);
	dr16->data.Mouse.Y= ((int16_t)buffer[8]) | ((int16_t)buffer[9] << 8);
	dr16->data.Mouse.Z = ((int16_t)buffer[8]) | ((int16_t)buffer[9] << 8);
	dr16->data.Mouse.Key_L = buffer[12];
	dr16->data.Mouse.Key_R = buffer[13];
	
	dr16->data.Keyboard.Data = ((int16_t)buffer[14]) | ((int16_t)buffer[15] << 8);
}



/// @brief 处理接收相关操作需放到串口接收中断里
/// @param
void Dr16_Rx_Callback(Dr16TypeDef *dr16, uint8_t *Buffer, uint8_t size)
{
    DJI_DR16_Data_Process(dr16, Buffer, size);
    DJI_DR16_Init(dr16);
    dr16->count = 50;
}


void Dr16_Timing_Callback(Dr16TypeDef *dr16)
{
	if (dr16->count > 0)
    {
        dr16->count--;
    }
}


uint8_t DR16_Get_Count(Dr16TypeDef *dr16)
{
	return dr16->count;
}

