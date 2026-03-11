/* ====== 应用层 ====== */
#include "public.h"

/* ====== 辅助层 ====== */	


/* ===== 公共回调层 ==== */
#include "tim_callback.h"

/* ====== 模块层 ====== */






void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	
	if(htim->Instance == TIM1)
	{
        Public_Cmd_Timing_Callback();
	}	
}

