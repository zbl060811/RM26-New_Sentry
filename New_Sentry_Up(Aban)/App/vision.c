#include "vision.h"



VisionTypeDef Vision;






void Vision_Init(void)
{
    // 清零结构体
//    memset(&Vision, 0, sizeof(VisionTypeDef)); 

//    Vision.flag = 1;
//    Vision.tick = 0;

//	HAL_UARTEx_ReceiveToIdle_DMA(&VISION_USE_USART, vision_buff, VISION_BUFFER_LENGTH);		// 串口空闲中断接收DMA
//    __HAL_DMA_DISABLE_IT(&VISION_USE_USART_RX_DMA, DMA_IT_HT);		// 关闭DMA接收一半中断
}


void Vision_Task(void)
{
    if(!Vision.flag || Vision.tick < VISION_TIME_TICK){
        return;
    }

    Vision.tick = 0;
}



void Vision_Rx_Callback(void)
{
//	HAL_UART_DMAStop(&VISION_USE_USART);

//    

//    // 重新启动DMA接收
//    HAL_UARTEx_ReceiveToIdle_DMA(&VISION_USE_USART, vision_buff, VISION_BUFFER_LENGTH);
//	__HAL_DMA_DISABLE_IT(&VISION_USE_USART_RX_DMA, DMA_IT_HT);		// 关闭DMA接收一半中断
}

void Vision_Timing_Callback(void)
{ 
    if(Vision.tick < VISION_TIME_TICK){
        Vision.tick++;
    }
}

