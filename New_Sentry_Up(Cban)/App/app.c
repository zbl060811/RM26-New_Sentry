/* ====== 应用层 ====== */
#include "app.h"		
#include "robot.h"
#include "gimbal.h"
#include "shoot.h"
#include "public_cmd.h"
#include "vision.h"


/* ====== 辅助层 ====== */
#include "pid.h"		
#include "vofa.h"
#include "filter.h"
#include "log.h"
#include "rm_typedef.h"
#include "config.h"

/* ===== 公共回调层 ==== */
#include "usart_callback.h"
#include "can_callback.h"
#include "tim_callback.h"

/* ====== 模块层 ====== */
#include "at9s.h"
#include "dji_motor.h"
#include "mg_motor.h"
#include "hi14.h"
#include "led.h"
#include "dr16.h"
#include "buzzer.h"
#include "communicate.h"



/* ====== 驱动层 ====== */
#include "bsp_can.h"
#include "bsp_usart.h"
#include "bsp_tim.h"



AppStateEnum AppState = STATE_NONE;

HAL_StatusTypeDef ret;

void App_Init(void)
{
	// bsp层初始化
	Bsp_Can_Init();
	Bsp_Tim_Init();
	Bsp_Usart_Init();

	// 模块层初始化
	Dji_Motor_Init();

	#if CONFIG_USE_REMOTE
		DJI_DR16_Init(&Dr16);
	#else
		At9s_Init();
	#endif

	Hi14_Init();
	Led_Init();
	Buzzer_Init();
	Communicate_Init();      
	Mg_Motor_Init(&mg_motor);
	
	
	// 应用层初始化
	Gimbal_Init();
	Shoot_Init();
	Public_Cmd_Init();
	Robot_Init();

	#if CONFIG_VISION_TO_ECU
	Vision_Init();
	#else
	Vofa_Init();
	#endif
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
			AppState = STATE_NORMAL;
			
		break;
		
		case STATE_NORMAL:
			
			#if CONFIG_VISION_TO_ECU
				Vision_Task();
			#else
				Vofa_Task();
			#endif

			
			#if CONFIG_USE_REMOTE
				if(DR16_Get_Count(&Dr16) == 0)
				{
					Dr16.error_tick++;
					if(Dr16.error_tick > 10)
					{
						AppState = STATE_STOP;	
						Dr16.error_tick = 0;
						Dr16.status = 0;		// 无信号
					}
				}
				else
				{
					// 应用层任务应放到最后调用
					Robot_Task();
					Gimbal_Task();
					Shoot_Task();
					Dr16.error_tick = 0;
					Dr16.status = 1;
				}
			#else
				Robot_Task();
				Gimbal_Task();
				Shoot_Task();	
			#endif
				
			Led_Normal_Task();	
			Buzzer_Normal_Task();
		break;
		
		case STATE_STOP:
			
			if(DR16_Get_Count(&Dr16))
			{
				AppState = STATE_NORMAL;
				Dr16.status = 1;		// 恢复信号
			}
		
			Led_Error_Task();
			Buzzer_Error_Task();
		break;
		
		case STATE_ERROR:
			
			
		
		break;
	}

	Public_Cmd_Task();
}


