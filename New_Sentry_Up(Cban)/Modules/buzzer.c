#include "buzzer.h"



BuzzerTypeDef Buzzer;



void Buzzer_Init(void)
{
	Buzzer.flag = 1;
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);		// start htim4 channel3 PWM output
}


void Buzzer_Timing_Callback(void)
{
	if(Buzzer.tick < BUZZER_TICK_TIME){
		Buzzer.tick++;
	}
}


void Buzzer_Normal_Task(void)
{
	if(!Buzzer.flag || Buzzer.tick < BUZZER_TICK_TIME){ 
		return;
	}
	
	Buzzer.tick = 0;
	
	BUZZER(0);
}



void Buzzer_Error_Task(void)
{
	if(Buzzer.flag == 0)	return;
	if(Buzzer.tick < BUZZER_TICK_TIME)	return;
	
	static uint8_t i = 0;
	Buzzer.tick = 0;
	
	BUZZER(i);
	if(i==0) i=1;
	else i=0;
}

