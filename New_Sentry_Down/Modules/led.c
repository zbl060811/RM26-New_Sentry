#include "led.h"



LedTypeDef Led;



void Led_Init(void)
{
	Led.flag = 1;
	
	BLUE_LED(0);
	GREEN_LED(0);
	RED_LED(0);
}


void Led_Timing_Callback(void)
{
	if(Led.tick < LED_TICK_TIME){
		Led.tick++;
	}
}


void Led_Normal_Task(void)
{
	if(Led.flag == 0)	return;
	if(Led.tick < LED_TICK_TIME)	return;
	
	Led.tick = 0;
	
	RED_LED(0);
	GREEN_LED(1);
}


void Led_Error_Task(void)
{
	if(Led.flag == 0)	return;
	if(Led.tick < LED_TICK_TIME)	return;
	
	Led.tick = 0;
	
	GREEN_LED(0);
	HAL_GPIO_TogglePin(RED_LED_GPIO_PORT, RED_LED_GPIO_PIN);
}
	
	
