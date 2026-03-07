#include "at9s.h"
#include "shoot.h"
#include "communicate.h"


At9sTypeDef At9s;



void At9s_Init(void)
{
	At9s.flag = 1;
	HAL_UARTEx_ReceiveToIdle_DMA(&huart3, At9s.at9s_data.dbus_buff, SBUS_FRAME_SIZE);		// 串口空闲中断接收DMA
	__HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);		// 关闭DMA接收一半中断
	At9s.fail_safe = 1;  // 故障保护标志位
    At9s.lost_frame = 1; // 丢帧标志位
	At9s.status = 1;	

	// 配置摇杆一阶低通过滤器
	for(uint8_t i=0;i<4;i++){
		At9s.joystick_lpf_filter[i].type = FILTER_LPF_FIRST_ORDER;
	}
	Lpf_Config(&At9s.joystick_lpf_filter[AT9S_CHANNEL_RIGHT_X].filter_type.lpf, 0.1f, 0.0f);
	Lpf_Config(&At9s.joystick_lpf_filter[AT9S_CHANNEL_RIGHT_Y].filter_type.lpf, 0.1f, 0.0f);
	Lpf_Config(&At9s.joystick_lpf_filter[AT9S_CHANNEL_LEFT_Y].filter_type.lpf, 0.1f, 0.0f);
	Lpf_Config(&At9s.joystick_lpf_filter[AT9S_CHANNEL_LEFT_X].filter_type.lpf, 0.1f, 0.0f);
}


void At9s_Parse_Frame(void)
{
	// 检查SBUS故障保护标志
    uint8_t sbus_flags = At9s.at9s_data.dbus_buff[23];
    At9s.fail_safe = (sbus_flags & 0x08) >> 3;  // 故障保护标志位
    At9s.lost_frame = (sbus_flags & 0x04) >> 2; // 丢帧标志位

	At9s.status = (At9s.fail_safe || At9s.lost_frame) ? 1 : 0; // 0:正常 1:断开

	// 解析16个通道
    At9s.at9s_data.raw_channels[0]  = (At9s.at9s_data.dbus_buff[1]       | (At9s.at9s_data.dbus_buff[2]  << 8))                     & 0x07FF;
    At9s.at9s_data.raw_channels[1]  = ((At9s.at9s_data.dbus_buff[2] >>3) | (At9s.at9s_data.dbus_buff[3]  << 5))                     & 0x07FF;
    At9s.at9s_data.raw_channels[2]  = ((At9s.at9s_data.dbus_buff[3] >>6) | (At9s.at9s_data.dbus_buff[4]  << 2) | (At9s.at9s_data.dbus_buff[5] << 10)) & 0x07FF;
    At9s.at9s_data.raw_channels[3]  = ((At9s.at9s_data.dbus_buff[5] >>1) | (At9s.at9s_data.dbus_buff[6]  << 7))                     & 0x07FF;
    At9s.at9s_data.raw_channels[4]  = ((At9s.at9s_data.dbus_buff[6] >>4) | (At9s.at9s_data.dbus_buff[7]  << 4))                     & 0x07FF;
    At9s.at9s_data.raw_channels[5]  = ((At9s.at9s_data.dbus_buff[7] >>7) | (At9s.at9s_data.dbus_buff[8]  << 1) | (At9s.at9s_data.dbus_buff[9] << 9))  & 0x07FF;
    At9s.at9s_data.raw_channels[6]  = ((At9s.at9s_data.dbus_buff[9] >>2) | (At9s.at9s_data.dbus_buff[10] << 6))                     & 0x07FF;
    At9s.at9s_data.raw_channels[7]  = ((At9s.at9s_data.dbus_buff[10]>>5) | (At9s.at9s_data.dbus_buff[11] << 3))                     & 0x07FF;
    At9s.at9s_data.raw_channels[8]  = (At9s.at9s_data.dbus_buff[12]      | (At9s.at9s_data.dbus_buff[13] << 8))                     & 0x07FF;
    At9s.at9s_data.raw_channels[9]  = ((At9s.at9s_data.dbus_buff[13]>>3) | (At9s.at9s_data.dbus_buff[14] << 5))                     & 0x07FF;
    At9s.at9s_data.raw_channels[10] = ((At9s.at9s_data.dbus_buff[14]>>6) | (At9s.at9s_data.dbus_buff[15] << 2) | (At9s.at9s_data.dbus_buff[16] << 10)) & 0x07FF;
    At9s.at9s_data.raw_channels[11] = ((At9s.at9s_data.dbus_buff[16]>>1) | (At9s.at9s_data.dbus_buff[17] << 7))                     & 0x07FF;
    At9s.at9s_data.raw_channels[12] = ((At9s.at9s_data.dbus_buff[17]>>4) | (At9s.at9s_data.dbus_buff[18] << 4))                     & 0x07FF;
    At9s.at9s_data.raw_channels[13] = ((At9s.at9s_data.dbus_buff[18]>>7) | (At9s.at9s_data.dbus_buff[19] << 1) | (At9s.at9s_data.dbus_buff[20] << 9))  & 0x07FF;
    At9s.at9s_data.raw_channels[14] = ((At9s.at9s_data.dbus_buff[20]>>2) | (At9s.at9s_data.dbus_buff[21] << 6))                     & 0x07FF;
    At9s.at9s_data.raw_channels[15] = ((At9s.at9s_data.dbus_buff[21]>>5) | (At9s.at9s_data.dbus_buff[22] << 3))                     & 0x07FF;
	
	// 使用滤波器对摇杆通道进行滤波处理
	// for(uint8_t i = 0; i < 4; i++){
	// 	At9s.at9s_data.raw_channels[i] = (uint16_t)Lpf_Update(&At9s.joystick_lpf_filter[i].filter_type.lpf, (float)At9s.at9s_data.raw_channels[i]);
	// }
	
	// 拨杆归一化
	At9s.at9s_rc.grade_a = At9s.at9s_data.raw_channels[5] == 306 ? 1 : 2;
	At9s.at9s_rc.grade_b = At9s.at9s_data.raw_channels[6] == 306 ? 1 : 2;
	if(At9s.at9s_data.raw_channels[7] == 306){
		At9s.at9s_rc.grade_d = 1;

		if(At9s.at9s_data.raw_channels[4] == 306) At9s.at9s_rc.grade_c = 1;
		if(At9s.at9s_data.raw_channels[4] == 1000) At9s.at9s_rc.grade_c = 3;
		if(At9s.at9s_data.raw_channels[4] == 1694) At9s.at9s_rc.grade_c = 2;
	}
	else if(At9s.at9s_data.raw_channels[7] == 1694){
		At9s.at9s_rc.grade_d = 2;

		if(At9s.at9s_data.raw_channels[4] == 653) At9s.at9s_rc.grade_c = 1;
		if(At9s.at9s_data.raw_channels[4] == 931) At9s.at9s_rc.grade_c = 3;
		if(At9s.at9s_data.raw_channels[4] == 1208) At9s.at9s_rc.grade_c = 2;
	}
	At9s.at9s_rc.grade_e = At9s.at9s_data.raw_channels[8] == 306 ? 1 : (At9s.at9s_data.raw_channels[8] == 1000 ? 3 : 2);
	At9s.at9s_rc.grade_g = At9s.at9s_data.raw_channels[9] == 292 ? 1 : (At9s.at9s_data.raw_channels[9] == 1008 ? 3 : 2);

	// 将最小值和最大值同时剔除一部分,再进行归一化
	At9s.at9s_rc.right_x = NORMALIZE(At9s.at9s_data.raw_channels[0], 306, 1694);
	At9s.at9s_rc.right_y = NORMALIZE(At9s.at9s_data.raw_channels[1], 306, 1694);
	At9s.at9s_rc.left_y = NORMALIZE(At9s.at9s_data.raw_channels[2], 306, 1694);
	At9s.at9s_rc.left_x = NORMALIZE(At9s.at9s_data.raw_channels[3], 306, 1694);


	
	// 将归一化值进行死区滤波,保持数据平稳
//	At9s.at9s_rc.right_x = DeadZone_Filter(At9s.at9s_rc.right_x, 0.05);
//	At9s.at9s_rc.right_y = DeadZone_Filter(At9s.at9s_rc.right_y, 0.05);
//	At9s.at9s_rc.left_y =  DeadZone_Filter(At9s.at9s_rc.left_y, 0.05);
//	At9s.at9s_rc.left_x = DeadZone_Filter(At9s.at9s_rc.left_x, 0.05);

	// 最后进行一次低通滤波,进一步平滑数据
//	At9s.at9s_rc.right_x = Lpf_Update(&At9s.joystick_lpf_filter[AT9S_CHANNEL_RIGHT_X].filter_type.lpf, At9s.at9s_rc.right_x);
//	At9s.at9s_rc.right_y = Lpf_Update(&At9s.joystick_lpf_filter[AT9S_CHANNEL_RIGHT_Y].filter_type.lpf, At9s.at9s_rc.right_y);
//	At9s.at9s_rc.left_y = Lpf_Update(&At9s.joystick_lpf_filter[AT9S_CHANNEL_LEFT_Y].filter_type.lpf, At9s.at9s_rc.left_y);
//	At9s.at9s_rc.left_x = Lpf_Update(&At9s.joystick_lpf_filter[AT9S_CHANNEL_LEFT_X].filter_type.lpf, At9s.at9s_rc.left_x);
}
	

void At9s_Rx_Callback(void)
{
	At9s_Parse_Frame();
	At9s.tick = 0;
}


void At9s_Task(void)
{
	if(!At9s.flag || At9s.tick < AT9S_TICK_TIME){ 
		return;
	}
	
	At9s.tick = 0;
	
	// At9s_Parse_Frame();
}


void At9s_Timing_Callback(void)
{
	if(At9s.tick < AT9S_TICK_TIME){
		At9s.tick++;
	}
}
