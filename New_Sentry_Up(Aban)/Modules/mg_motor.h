#ifndef __MG_MOTOR_H__
#define __MG_MOTOR_H__



#include "bsp_can.h"



#pragma pack(1)

typedef enum Mg_Motor_Cmd_Enum_T
{
	MG_CLOSER_CMD = 0X80,			// 电机关闭
	MG_STOP_CMD = 0X81,				// 电机暂停
	MG_OPEN_CMD = 0X88,				// 电机运行

	MG_TORQUE_CLOSE_CMD = 0XA1,		// 转矩闭环控制
	MG_SPEED_CLOSE_CMD = 0XA2,		// 速度闭环控制
	MG_MULTI_TURN_POSITION_CLOSE_1_CMD = 0XA3,		// 多圈位置闭环控制(无速度控制)
	MG_MULTI_TURN_POSITION_CLOSE_2_CMD = 0XA4,		// 多圈位置闭环控制(含速度控制)
	MG_SINGLE_LAP_POSITION_CLOSE_1_CMD = 0XA5,		// 单圈位置闭环控制(无速度控制)
	MG_SINGLE_LAP_POSITION_CLOSE_2_CMD = 0XA6,		// 单圈位置闭环控制(含速度控制)
	MG_INCREMENTAL_POSITION_CLOSE_1_CMD = 0XA7,		// 增量位置闭环控制(无速度控制)
	MG_INCREMENTAL_POSITION_CLOSE_2_CMD = 0XA8,		// 增量位置闭环控制(含速度控制)
	MG_READ_PID_DATA_CMD = 0X30,	// 读取PID参数
	MG_WRITE_PID_DATA_RAM_CMD = 0X31,	// 写PID参数到RAM
	MG_WRITE_PID_DATA_ROM_CMD = 0X32,	// 写PID参数到ROM
	MG_READ_ACCELERATION_CMD = 0X33,	// 读取加速度
	MG_WRITE_ACCELERATION_RAM_CMD = 0X34,  // 写加速度到RAM
	MG_READ_ENCODER_CMD = 0X90, 	// 读取编码器数据
	MG_WRITE_ENCODER_ROM_CMD = 0X91,	// 写编码器数据到ROM作为零点
	MG_WRITE_NOW_ROM_CMD = 0X19,	// 写编码器数据到ROM作为零点
	MG_READ_MULTI_TURN_ANGLE_CMD = 0X92,   // 读取多圈角度
	MG_READ_SINGLE_LAP_ANGLE_CMD = 0X94,   // 读取单圈角度
	MG_READ_MOTOR_1_STATUS_ERROR_CMD = 0X9A,	// 读取电机状态1(该命令读取温度、电压、错误标志)
	MG_CLEAR_ERROR_CMD = 0X9B,		// 清除电机错误标志
	MG_READ_MOTOR_2_STATUS_CMD = 0X9C,	// 读取电机状态2(该命令读取温度、电机、转速、编码器位置)
	MG_READ_MOTOR_3_STATUS_CMD = 0X9D	// 读取电机状态3(该命令读取温度、三相电流数据)
} MgMotorCmdEnum;


typedef struct Mg_Motor_Status_1_T
{
	uint8_t temperature;			// 电机温度
	uint8_t voltage;				// 电池电压
	uint8_t error_flag;				// 错误标志
} MgMotorStatus1TypeDef;

typedef struct Mg_Motor_Status_2_T
{
	uint8_t temperature;			// 电机温度
	uint8_t motor_flag;				// 电机标志
	uint16_t speed;					// 电机转速
	uint32_t encoder_position;		// 编码器位置
} MgMotorStatus2TypeDef;

typedef struct Mg_Motor_Status_3_T
{
	uint8_t temperature;			// 电机温度
	uint16_t current_a;				//三相电流A
	uint16_t current_b;				//三相电流B
	uint16_t current_c;				//三相电流C
} MgMotorStatus3TypeDef;


// MG电机发送ID
typedef enum __MG_Motor_ID      
{
    MG_Motor_ID_1 = 0X141,     
    MG_Motor_ID_2 = 0X142,      
} MGMotorIDEnum;   

// 电机控制参数结构体
typedef struct 
{
	CAN_HandleTypeDef *hcan;
    MGMotorIDEnum motor_id;       // 电机ID
    int32_t angle;          // 目标角度（单位：0.01度）
    int16_t speed;      // 最大速度（单位：0.01度/秒）
} MotorControlParamsTypeDef;   // MG电机控制参数结构体

/// @brief MG电机结构体
/// @param flag 标志位，用于指示电机是否处于运行状态
/// @param count 计数器，用于记录电机运行的时间
/// @param MG_Motor 电机控制参数
typedef struct
{
	uint16_t speed_max;		// 最大输出速度

	int16_t angle_min;		// 最小输出角度
	int16_t angle_max;		// 最大输出角度
	
    MotorControlParamsTypeDef params;     // 左MG电机控制参数
} MGMotorTypeDef;


extern MGMotorTypeDef mg_motor;


#pragma pack()








/* *************** Mg_Motor Function *************** */
void Mg_Motor_Init(MGMotorTypeDef *Motor);
HAL_StatusTypeDef MG_Motor_Run(MotorControlParamsTypeDef *params);
HAL_StatusTypeDef MG_Motor_SendCommand(MotorControlParamsTypeDef *params,const uint8_t *data,uint8_t data_len);
HAL_StatusTypeDef MG_Motor_PositionControl(MGMotorTypeDef *Motor);




#endif

