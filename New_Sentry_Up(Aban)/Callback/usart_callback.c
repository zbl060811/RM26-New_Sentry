#include "usart_callback.h"

#include "config.h"

#include "vision.h"

#include "at9s.h"
#include "hi14.h"
#include "vofa.h"
#include "dr16.h"



void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance == USART6)
	{
		Hi14_Rx_Callback(Size);
	}

	if(huart->Instance == USART1)
	{
		#if CONFIG_USE_REMOTE
		Dr16_Rx_Callback(&Dr16, Dr16.buffer, Size);
		#else
		At9s_Rx_Callback();
		#endif
	}

	if(huart->Instance == UART8)
	{	
		#if CONFIG_VISION_TO_ECU
		Vision_Rx_Callback();
		#else
		Vofa_Rx_Callback(Size);
		#endif
	}
}



void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART6)
    {
//        printf("[UART Error] error code: 0x%08lX\r\n", huart->ErrorCode);
		
//        __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_ORE | UART_FLAG_FE | 
//                                     UART_FLAG_NE | UART_FLAG_PE);
//        
//        HAL_UART_DMAStop(&huart1);
//        HAL_UARTEx_ReceiveToIdle_DMA(&VISION_USE_USART, vision_buff, VISION_BUFFER_LENGTH);
    }
}

