#ifndef __LOG_H__
#define __LOG_H__



#include <stdio.h>
#include "bsp_usart.h"



/* ====== 总开关 ====== */
#define 	LOG_ENABLE			0


/* ====== 任务/模块开关（按需增删）====== */
#define 	LOG_ON_LED			1
#define 	LOG_ON_BUZZER		1
#define 	LOG_ON_AT9S			1
#define 	LOG_ON_DJI_MOTOR	1
#define 	LOG_ON_SUPER_CAP	1


/* ====== 内核打印函数（如需改成SEGGER_RTT_Printf，这里替换即可） ====== */
#ifndef LOG_PRINTF
#define LOG_PRINTF printf
#endif


/* ====== 核心输出宏（带模块名与源码位置信息） ====== */
#if LOG_ENABLE
  #define __LOG_PRINT(mod, fmt, ...)                    \
    do {                                                \
      LOG_PRINTF("[%s] %s:%d: " fmt "\r\n",             \
                 mod, __FUNCTION__, __LINE__,           \
                 ##__VA_ARGS__);                        \
    } while (0)
#else
  /* 总开关关闭时，全部变空操作，参数不会被求值（编译期裁剪） */
  #define __LOG_PRINT(mod, fmt, ...)  ((void)0)
#endif
  
  

/* ====== 每任务的便捷宏（按需增删）====== */
#if LOG_ENABLE && LOG_ON_LED
  #define LOG_LED(fmt, ...) __LOG_PRINT("LED", fmt, ##__VA_ARGS__)
#else
  #define LOG_LED(...)      ((void)0)
#endif
  
#if LOG_ENABLE && LOG_ON_BUZZER
  #define LOG_BUZZER(fmt, ...)   __LOG_PRINT("BUZZER", fmt, ##__VA_ARGS__)
#else
  #define LOG_BUZZER(...)        ((void)0)
#endif

#if LOG_ENABLE && LOG_ON_AT9S
  #define LOG_AT9S(fmt, ...)   __LOG_PRINT("AT9S", fmt, ##__VA_ARGS__)
#else
  #define LOG_AT9S(...)        ((void)0)
#endif

#if LOG_ENABLE && LOG_ON_DJI_MOTOR
  #define LOG_DJI_MOTOR(fmt, ...)   __LOG_PRINT("DJI_MOTOR", fmt, ##__VA_ARGS__)
#else
  #define LOG_DJI_MOTOR(...)        ((void)0)
#endif

#if LOG_ENABLE && LOG_ON_SUPER_CAP
  #define LOG_SUPER_CAP(fmt, ...)   __LOG_PRINT("SUPER_CAP", fmt, ##__VA_ARGS__)
#else
  #define LOG_SUPER_CAP(...)        ((void)0)
#endif





#endif

