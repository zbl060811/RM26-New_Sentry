#include "usart_callback.h"

#include "hi12.h"

#include "referee.h"


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart == &huart1)
	{
//		Hi12_Rx_Callback(Size);
	}
	else if(huart == &huart6)
	{
		 Referee_RX_Callback(Referee_Get_Buffer(), Size);
	}
}

