#include "tim_callback.h"

#include "chassis.h"

#include "vofa.h"

#include "at9s.h"
#include "led.h"
#include "buzzer.h"
#include "hi12.h" 
#include "communicate.h"



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	
	if(htim->Instance == TIM1)
	{
		Chassis_Timing_Callback();
		
		Vofa_Timing_Callback();
		
		At9s_Timing_Callback();
		
		Led_Timing_Callback();
		
		Buzzer_Timing_Callback();
		
		Hi_Timing_Callback();

		Communicate_Timing_Callback();
	}	
}

