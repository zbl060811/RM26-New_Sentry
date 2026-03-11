#ifndef __APP_H__
#define __APP_H__





typedef enum App_State_Enum_T
{
	STATE_NONE,
	STATE_INIT,
	STATE_NORMAL,
	STATE_STOP,
	STATE_ERROR
} AppStateEnum;







/* *************** App Function *************** */
void App_Init(void);
void App_Task(void);





#endif


