#ifndef __ET16S_H__
#define __ET16S_H__



#include "usart.h"
#include "rm_typedef.h"





#define SBUS_FRAME_SIZE		25		// SBUS数据大小
#define CHANNELS_NUM		16		// 通道数
#define ET16S_TICK_TIME		10	

#define RC_DATA_FILTER		20		// 通道过滤值





#pragma pack(1)



typedef struct Et16s_Data_Struct_T
{
	uint8_t dbus_buff[SBUS_FRAME_SIZE];
	
	/*
		通道0——右舵X（306-1694）
		通道1——右舵Y（306-1694）
		通道2——左舵Y（306-1694）
		通道3——左舵X（306-1694）
		通道4——C档（306/1000/1694） 当d为up时c:306/1000/1694,当d为down时c:653/931/1208
		通道5——A档（306/1694）
		通道6——B档（306/1694）
		通道7——D档（306/1694）		up:306/down:1694(当为up时c:306/1000/1694,当为down时c:653/931/1208)
		通道8——E档（306/1000/1694）
		通道9——G档（292/1008/1692）
		其他通道暂时忽略
	*/
	uint16_t raw_channels[CHANNELS_NUM];
} Et16sDataTypeDef;

typedef struct Et16s_Rc_Struct_T
{
	// 遥控数据
	float left_x;
	float left_y;
	float right_x;
	float right_y;
	
	// 拨片数据
	/*
		两档：Up:1/Down:2
		三档：Up:1/Mid:3/Down:2 
	 */
	uint8_t grade_a;		
	uint8_t grade_b;		// 绑定运动模式
	uint8_t grade_c;		// 绑定发射模式
	uint8_t grade_d;		
	uint8_t grade_e;		// 绑定控制方式
	uint8_t grade_g;		
} Et16sRcTypeDef;

typedef struct Et16s_Struct_T
{
	uint8_t flag;
	uint32_t tick;
	uint8_t fail_safe;
	uint8_t lost_frame;
	uint8_t status;			// 0:正常 1:断开
	
	Et16sDataTypeDef at9s_data;
	Et16sRcTypeDef at9s_rc;
} Et16sTypeDef;





#pragma pack()


/* *************** ET16S Function *************** */
void Et16s_Init(void);
void Et16s_Parse_Frame(void);
void Et16s_Rx_Callback(void);
void Et16s_Task(void);
void Et16s_Timing_Callback(void);




#endif
