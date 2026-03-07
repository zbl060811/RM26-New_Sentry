#ifndef __VOFA_H__
#define __VOFA_H__


#include "rm_typedef.h"
#include "bsp_usart.h"


#define JUSTFLOAT_CH_COUNT  	10
#define FIREWATER_CH_COUNT		1	
#define VOFA_BUFFER_LENGTH		40	
#define VOFA_USE_USART			huart6



#define VOFA_TICK_TIME			10		



#pragma pack(1)

typedef struct JustFloat_Tx_Struct_T
{
	float fdata[JUSTFLOAT_CH_COUNT];
	const uint8_t *tail;
	
	uint8_t fbuff[VOFA_BUFFER_LENGTH];
} JustFloatTxTypeDef;

typedef struct FireWater_Tx_Struct_T
{
	uint8_t wdata[FIREWATER_CH_COUNT];
	const uint8_t *tail;
	
	uint8_t wbuff[VOFA_BUFFER_LENGTH];
} FireWaterTxTypeDef;


typedef struct Vofa_JustFloat_Struct_T
{
	uint8_t flag;
	uint32_t tick;
	
	JustFloatTxTypeDef tx_float;
	FireWaterTxTypeDef tx_water;
	
	
} VofaTypeDef;


#pragma pack()



/* *************** VOFA Function *************** */
void Vofa_Init(void);
void Vofa_Task(void);
void Vofa_Timing_Callback(void);



#endif

