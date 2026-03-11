#include "bsp_uart.h"
#include <stdio.h>





// redirect printf
// int fputc(int ch, FILE *f)
// {
// 	HAL_UART_Transmit(&huart6, (uint8_t *)&ch, 1, 1000);
// 	return ch;
// }
 
// int fgetc(FILE *f)
// {
// 	uint8_t ch = 0;
// 	HAL_UART_Receive(&huart6, &ch, 1, 1000);
// 	return ch;
// }


