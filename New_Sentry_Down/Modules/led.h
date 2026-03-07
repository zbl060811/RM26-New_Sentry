#ifndef __LED_H__
#define __LED_H__


#include "gpio.h"



#define LED_TICK_TIME		100	



/* *************** Led Pin define *************** */
#define		BLUE_LED_GPIO_PORT		GPIOH
#define 	BLUE_LED_GPIO_PIN		GPIO_PIN_10

#define		GREEN_LED_GPIO_PORT		GPIOH
#define 	GREEN_LED_GPIO_PIN		GPIO_PIN_11

#define		RED_LED_GPIO_PORT		GPIOH
#define 	RED_LED_GPIO_PIN		GPIO_PIN_12


#define 	BLUE_LED(x)		HAL_GPIO_WritePin(BLUE_LED_GPIO_PORT, BLUE_LED_GPIO_PIN, (GPIO_PinState)(x))
#define 	GREEN_LED(x)	HAL_GPIO_WritePin(GREEN_LED_GPIO_PORT, GREEN_LED_GPIO_PIN, (GPIO_PinState)(x))
#define 	RED_LED(x)		HAL_GPIO_WritePin(RED_LED_GPIO_PORT, RED_LED_GPIO_PIN, (GPIO_PinState)(x))


typedef struct Led_Struct_T
{
	uint8_t flag;
	uint32_t tick;
	
	
} LedTypeDef;



/* *************** Led Function *************** */
void Led_Init(void);
void Led_Timing_Callback(void);
void Led_Error_Task(void);
void Led_Normal_Task(void);




#endif

