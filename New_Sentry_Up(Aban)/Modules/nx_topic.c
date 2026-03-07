#include "nx_topic.h"



VisionTopicTyepDef vision_topic;





void Vision_Topic_Init(void)
{
	HAL_UARTEx_ReceiveToIdle_DMA(&NX_USE_USART, vision_topic.rx_buffer, NX_RX_BUFFER_LENGTH);
    __HAL_DMA_DISABLE_IT(&NX_USE_USART_RX_DMA,DMA_IT_HT);
}


void Vision_Topic_Data_Process(uint8_t *buff, uint8_t size)
{
	if(buff[0] == NX_RX_TAIL && size == NX_NAVIGATION)
	{
		
	}
}





