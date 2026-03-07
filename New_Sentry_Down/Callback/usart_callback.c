#include "usart_callback.h"

#include "at9s.h"
#include "hi12.h"


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart == &huart3)
	{
		At9s_Rx_Callback();
	}
	else if(huart == &huart1)
	{
		Hi12_Rx_Callback(Size);
	}
}

