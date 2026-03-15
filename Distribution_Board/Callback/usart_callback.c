#include "usart_callback.h"

#include "nx_topic.h"



void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart == &huart1)
	{
		Nx_Rx_Handler(Size);
	}
}


