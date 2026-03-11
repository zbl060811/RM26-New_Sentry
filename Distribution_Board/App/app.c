#include "app.h"

#include "public.h"

#include "comm.h"
#include "nx_topic.h"
#include "vofa.h"

#include "bsp_can.h"
#include "bsp_tim.h"



AppStateEnum AppState = STATE_NONE;


void App_Init(void)
{
    // bsp层初始化
    Bsp_Can_Init();
	Bsp_Tim_Init();
	
    // App层初始化
    Public_Cmd_Init();
	
	// 模块层初始化
	Communicate_Init();
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

			AppState = STATE_NORMAL;

            break;
        
        case STATE_NORMAL:

            Public_Cmd_Task();

            break;
        
        case STATE_STOP:
            break;
        
        case STATE_ERROR:
            break;
        
        default:
            break;
    }
    
}


