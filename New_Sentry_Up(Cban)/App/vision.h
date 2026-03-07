#ifndef __VISION_H__
#define __VISION_H__




#include "vision_topic.h"



#define VISION_TIME_TICK    10



#pragma pack(1)


typedef struct 
{
    uint8_t flag;
    uint32_t tick;

    uint16_t rx_data_len;
} VisionTypeDef;




extern uint8_t vision_buff[VISION_BUFFER_LENGTH];


#pragma pack()



/* *************** Vision Function *************** */
void Vision_Init(void);
void Vision_Task(void);
void Vision_Rx_Callback(void);
void Vision_Timing_Callback(void);







#endif

