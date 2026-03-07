#include "bsp_usart.h"
#include <stdio.h>



// redirect printf
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit_DMA(&huart6, (uint8_t *)&ch, 1);
	return ch;
}
 
int fgetc(FILE *f)
{
	uint8_t ch = 0;
	HAL_UART_Receive_DMA(&huart6, &ch, 1);
	return ch;
}


