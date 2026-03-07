/* ====== 应用层 ====== */
#include "robot.h"
#include "gimbal.h"
#include "shoot.h"
#include "public_cmd.h"
#include "vision.h"

/* ====== 辅助层 ====== */	
#include "vofa.h"

/* ===== 公共回调层 ==== */
#include "tim_callback.h"

/* ====== 模块层 ====== */
#include "at9s.h"
#include "hi14.h"
#include "dr16.h"






void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	
	if(htim->Instance == TIM1)
	{
		Robot_Timing_Callback();
		Gimbal_Timing_Callback();
		Shoot_Timing_Callback();
		Public_Cmd_Timing_Callback();
		Vision_Timing_Callback();

		Vofa_Timing_Callback();
		
//		At9s_Timing_Callback();
		Hi14_Timing_Callback();
		Dr16_Timing_Callback(&Dr16);
	}	
}

