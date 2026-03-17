/* ====== 应用层 ====== */
#include "app.h"		
#include "chassis.h"

/* ====== 辅助层 ====== */
#include "pid.h"		
#include "vofa.h"
#include "log.h"
#include "rm_typedef.h"

/* ===== 公共回调层 ==== */
#include "usart_callback.h"
#include "can_callback.h"
#include "tim_callback.h"

/* ====== 模块层 ====== */
#include "buzzer.h"
#include "led.h"
#include "dji_motor.h"
#include "super_cap.h"
#include "hi12.h"
#include "communicate.h"
#include "referee.h"

/* ====== 模块层 ====== */
#include "bsp_can.h"
#include "bsp_usart.h"
#include "bsp_tim.h"


 #define DEBUG_TEST 	1

AppStateEnum AppState = STATE_NONE;



void App_Init(void)
{
	// bsp层初始化
	Bsp_Can_Init();
	Bsp_Tim_Init();
	
	// 模块层初始化
	Dji_Motor_Init();
	Led_Init();
	Buzzer_Init();
	Hi12_Init();
	Communicate_Init();
	RefereeInit();
	
	// 辅助层初始化
	Vofa_Init();
	
	// 应用层初始化
	Chassis_Init();
}


void App_Task(void)
{
	switch((uint8_t) AppState )
	{
		case STATE_NONE:
	
			AppState = STATE_INIT;
		
		break;
		
		case STATE_INIT:
			
			App_Init();

			// 参数设置
			Super_Cap_Set_Power(100.0f);
			AppState = STATE_NORMAL;
		
		break;
		
		case STATE_NORMAL:
			
			if(Can_Communicate.data.rc_status == 1)
			{
				Vofa_Task();
							
				// APP任务应放到最后调用
				#if DEBUG_TEST
					Chassis_Task();
				#endif
			} else
			{
				AppState = STATE_STOP;
				
			}
				
			Led_Normal_Task();	
			Buzzer_Normal_Task();
		break;
		
		case STATE_STOP:
			
			if(Can_Communicate.data.rc_status == 1)
			{
				AppState = STATE_NORMAL;
			}
		
			Led_Error_Task();
			Buzzer_Error_Task();
		break;
		
		case STATE_ERROR:
			
			
		
		break;
	}
	Communicate_Task();
}

