#ifndef __DJI_DR16_H__
#define __DJI_DR16_H__


#include "rm_typedef.h"

#define DJI_DR16_DATA_LENGHT 18

#pragma pack(1) // 指定结构体按照1字节对齐
typedef struct __DJI_DR16_Struct
{
    struct __RC_Struct
    {
        uint16_t CH0;
        uint16_t CH1;
        uint16_t CH2;
        uint16_t CH3;
        uint16_t Wheel;
        uint8_t S1;
        uint8_t S2;
    } RC;

    struct __RC_Value_Struct
    {
        float CH0;
        float CH1;
        float CH2; // 左摇杆X轴
        float CH3;
        float Wheel;
        uint8_t S1;
        uint8_t S2;
    } RC_Value;

    struct __Mouse_Struct
    {
        int16_t X;
        int16_t Y;
        int16_t Z;
        uint8_t Key_L;
        uint8_t Key_R;
    } Mouse;

    union __Keyboard_Union
    {
        struct __Keyboard_Struct
        {
            uint8_t W : 1;
            uint8_t S : 1;
            uint8_t A : 1;
            uint8_t D : 1;
            uint8_t Q : 1;
            uint8_t E : 1;
            uint8_t Shift : 1;
            uint8_t Ctrl : 1;
            uint8_t None;
        } Key;

        uint16_t Data;
    } Keyboard;
} Dr16DataTypeDef;

typedef struct 
{
	Dr16DataTypeDef data;
	
	uint8_t flag;
	uint32_t count;
    uint32_t error_tick;
    uint8_t status;
	uint8_t buffer[DJI_DR16_DATA_LENGHT];
} Dr16TypeDef;

#pragma pack() // 取消结构体对齐


extern Dr16TypeDef Dr16;


void DJI_DR16_Init(Dr16TypeDef *dr16);
uint8_t *DJI_DR16_Get_Buffer(Dr16TypeDef *dr16);
void DJI_DR16_Data_Process(Dr16TypeDef *dr16, uint8_t *buffer, uint8_t size);
void Dr16_Rx_Callback(Dr16TypeDef *dr16, uint8_t *Buffer, uint8_t size);
uint8_t DR16_Get_Count(Dr16TypeDef *dr16);
void Dr16_Timing_Callback(Dr16TypeDef *dr16);


#endif

