#ifndef __AT9S_H__
#define __AT9S_H__


#include "usart.h"
#include "rm_typedef.h"
#include "filter.h"



#define SBUS_FRAME_SIZE		25		// SBUS数据大小
#define CHANNELS_NUM		16		// 通道数
#define AT9S_TICK_TIME		5	

#define RC_DATA_FILTER		20		// 通道过滤值


#pragma pack(1)

typedef enum At9s_Channels_Enum_T
{
	AT9S_CHANNEL_RIGHT_X = 0,
	AT9S_CHANNEL_RIGHT_Y,
	AT9S_CHANNEL_LEFT_Y,
	AT9S_CHANNEL_LEFT_X,
	AT9S_CHANNEL_GRADE_C,
	AT9S_CHANNEL_GRADE_A,
	AT9S_CHANNEL_GRADE_B,
	AT9S_CHANNEL_GRADE_D,
	AT9S_CHANNEL_GRADE_E
} At9sChannelsEnum;


typedef struct At9s_Data_Struct_T
{
	uint8_t dbus_buff[SBUS_FRAME_SIZE];
	
	/*
		通道0——右舵X（306-1694）
		通道1——右舵Y（306-1694）
		通道2——左舵Y（306-1694）
		通道3——左舵X（306-1694）
		通道4——C档（306/1000/1694）
		通道5——A档（306/1694）
		通道6——B档（306/1694）
		通道7——D档（306/1694）
		通道8——E档（306/1000/1694）
		其他通道暂时忽略
	*/
	uint16_t raw_channels[CHANNELS_NUM];
} At9sDataTypeDef;

typedef struct At9s_Rc_Struct_T
{
	// 遥控数据
	float left_x;
	float left_y;
	float right_x;
	float right_y;
	
	// 拨片数据
	uint8_t grade_a;
	uint8_t grade_b;
	uint8_t grade_c;
	uint8_t grade_d;
	uint8_t grade_e;
} At9sRcTypeDef;

typedef struct At9s_Struct_T
{
	uint8_t flag;
	uint32_t tick;
	uint8_t fail_safe;
	uint8_t lost_frame;
	uint8_t status;		// 0:正常 1:断开
	
	At9sDataTypeDef at9s_data;
	At9sRcTypeDef at9s_rc;
	FilterTypeDef filter[4];
} At9sTypeDef;


extern At9sTypeDef At9s;

#pragma pack()



/* *************** AT9S Function *************** */
void At9s_Init(void);
void At9s_Parse_Frame(void);
uint8_t At9s_Get_Status(void);
void At9s_Rx_Callback(void);
void At9s_Task(void);
void At9s_Timing_Callback(void);



#endif

