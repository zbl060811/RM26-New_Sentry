#ifndef __BUZZER_H__
#define __BUZZER_H__


#include "tim.h"




/* *************** Buzzer Pin define *************** */
#define		BUZZER_GPIO_PORT		GPIOD
#define 	BUZZER_GPIO_PIN			GPIO_PIN_14


#define 	BUZZER(x)				__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, (x==1)? 10 : 0 )

#define 	BUZZER_TICK_TIME		500


typedef struct Buzzer_Struct_T
{
	uint8_t flag;
	uint32_t tick;
} BuzzerTypeDef;



/* *************** Buzzer Function *************** */
void Buzzer_Init(void);
void Buzzer_Timing_Callback(void);
void Buzzer_Error_Task(void);
void Buzzer_Normal_Task(void);


#endif
