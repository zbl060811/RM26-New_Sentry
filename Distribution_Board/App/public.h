#ifndef __PUBLIC_CMD_H__
#define __PUBLIC_CMD_H__



#include "comm.h"




#define PUBLIC_CMD_TICK_TIME   5



#pragma pack(1)

typedef enum Public_Cmd_Type_Enum_T
{
    CAN_MSG_NONE = 0x00,       // 无操作
    CAN_MSG_NX_NAVIGATION = 0X01,   // NX导航信息
    CAN_MSG_NX_VISINO = 0x02,       // NX视觉信息
} PublicCmdTypeEnum;



typedef struct Public_Eular_Struct_T
{
    PublicCmdTypeEnum cmd_type;   // 命令类型
    int16_t x;      
    int16_t y;        
    int16_t z;     
} PublicNavigationTypeDef;


typedef struct Public_Cmd_Struct_T
{
    uint8_t flag;
    uint32_t tick;

    CanCommunicateTypeDef can_com;
    PublicNavigationTypeDef navigation_data;
} PublicCmdTypeDef;



#pragma pack()


extern PublicCmdTypeDef Public;


/* *************** Public Cmd Function *************** */
void Public_Cmd_Init(void);
void Public_Cmd_Task(void);
void Public_Cmd_Timing_Callback(void);





#endif
