#include "et16s.h"




Et16sTypeDef Et16s;



void Et16s_Init(void)
{
	HAL_UARTEx_ReceiveToIdle_DMA(&huart3, Et16s.at9s_data.dbus_buff, SBUS_FRAME_SIZE);		// 串口空闲中断接收DMA
	__HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);		// 关闭DMA接收一半中断
}


void Et16s_Parse_Frame(void)
{
	// 检查SBUS故障保护标志
    uint8_t sbus_flags = Et16s.at9s_data.dbus_buff[23];
    Et16s.fail_safe = (sbus_flags & 0x08) >> 3;  // 故障保护标志位
    Et16s.lost_frame = (sbus_flags & 0x04) >> 2; // 丢帧标志位

	Et16s.status = (Et16s.fail_safe || Et16s.lost_frame) ? 1 : 0; // 0:正常 1:断开

	// 解析16个通道
    Et16s.at9s_data.raw_channels[0]  = (Et16s.at9s_data.dbus_buff[1]       | (Et16s.at9s_data.dbus_buff[2]  << 8))                     & 0x07FF;
    Et16s.at9s_data.raw_channels[1]  = ((Et16s.at9s_data.dbus_buff[2] >>3) | (Et16s.at9s_data.dbus_buff[3]  << 5))                     & 0x07FF;
    Et16s.at9s_data.raw_channels[2]  = ((Et16s.at9s_data.dbus_buff[3] >>6) | (Et16s.at9s_data.dbus_buff[4]  << 2) | (Et16s.at9s_data.dbus_buff[5] << 10)) & 0x07FF;
    Et16s.at9s_data.raw_channels[3]  = ((Et16s.at9s_data.dbus_buff[5] >>1) | (Et16s.at9s_data.dbus_buff[6]  << 7))                     & 0x07FF;
    Et16s.at9s_data.raw_channels[4]  = ((Et16s.at9s_data.dbus_buff[6] >>4) | (Et16s.at9s_data.dbus_buff[7]  << 4))                     & 0x07FF;
    Et16s.at9s_data.raw_channels[5]  = ((Et16s.at9s_data.dbus_buff[7] >>7) | (Et16s.at9s_data.dbus_buff[8]  << 1) | (Et16s.at9s_data.dbus_buff[9] << 9))  & 0x07FF;
    Et16s.at9s_data.raw_channels[6]  = ((Et16s.at9s_data.dbus_buff[9] >>2) | (Et16s.at9s_data.dbus_buff[10] << 6))                     & 0x07FF;
    Et16s.at9s_data.raw_channels[7]  = ((Et16s.at9s_data.dbus_buff[10]>>5) | (Et16s.at9s_data.dbus_buff[11] << 3))                     & 0x07FF;
    Et16s.at9s_data.raw_channels[8]  = (Et16s.at9s_data.dbus_buff[12]      | (Et16s.at9s_data.dbus_buff[13] << 8))                     & 0x07FF;
    Et16s.at9s_data.raw_channels[9]  = ((Et16s.at9s_data.dbus_buff[13]>>3) | (Et16s.at9s_data.dbus_buff[14] << 5))                     & 0x07FF;
    Et16s.at9s_data.raw_channels[10] = ((Et16s.at9s_data.dbus_buff[14]>>6) | (Et16s.at9s_data.dbus_buff[15] << 2) | (Et16s.at9s_data.dbus_buff[16] << 10)) & 0x07FF;
    Et16s.at9s_data.raw_channels[11] = ((Et16s.at9s_data.dbus_buff[16]>>1) | (Et16s.at9s_data.dbus_buff[17] << 7))                     & 0x07FF;
    Et16s.at9s_data.raw_channels[12] = ((Et16s.at9s_data.dbus_buff[17]>>4) | (Et16s.at9s_data.dbus_buff[18] << 4))                     & 0x07FF;
    Et16s.at9s_data.raw_channels[13] = ((Et16s.at9s_data.dbus_buff[18]>>7) | (Et16s.at9s_data.dbus_buff[19] << 1) | (Et16s.at9s_data.dbus_buff[20] << 9))  & 0x07FF;
    Et16s.at9s_data.raw_channels[14] = ((Et16s.at9s_data.dbus_buff[20]>>2) | (Et16s.at9s_data.dbus_buff[21] << 6))                     & 0x07FF;
    Et16s.at9s_data.raw_channels[15] = ((Et16s.at9s_data.dbus_buff[21]>>5) | (Et16s.at9s_data.dbus_buff[22] << 3))                     & 0x07FF;
}
	

void Et16s_Rx_Callback(void)
{
	Et16s_Parse_Frame();
	Et16s.tick = 0;
}


void Et16s_Task(void)
{
	if(!Et16s.flag || Et16s.tick < ET16S_TICK_TIME){ 
		return;
	}
	
	Et16s.tick = 0;
	
	
}


void Et16s_Timing_Callback(void)
{
	if(Et16s.tick < ET16S_TICK_TIME){
		Et16s.tick++;
	}
}
