#ifndef __DJI_MOTOR_H__
#define __DJI_MOTOR_H__



#include "bsp_can.h"


#pragma pack(1)


typedef enum Dji_Motor_Tx_ID_Enum_T
{
	DJI_MOTOR_TX_1_4,
	DJI_MOTOR_TX_5_8,
	DJI_MOTOR_TX_SUM
} DjiMotorTxIdEnum;

typedef enum Dji_Motor_Chassis_Group_Enum_T
{
	DJI_MOTOR_M3508_CHASSIS_RX_1,	
	DJI_MOTOR_M3508_CHASSIS_RX_2,
	DJI_MOTOR_M3508_CHASSIS_RX_3,
	DJI_MOTOR_M3508_CHASSIS_RX_4,
	DJI_MOTOR_CHASSIS_RX_SUM
} DjiMotorChassisGroupEnum;

typedef enum Dji_Motor_Chassis_Yaw_Group_Enum_T
{
	DJI_MOTOR_6020_CHASSIS_YAW_RX_1,
	DJI_MOTOR_CHASSIS_YAW_RX_SUM
} DjiMotorChassisYawGroupEnum;


typedef enum Dji_Motor_Group_Enum_T
{
	
	/* 
				底盘电机组
	   底盘四个全向轮电机
	*/
	DJI_CHASSIS_GROUP,		
	
	/* 
				腰盘电机组
	   大腰盘电机 
	*/
	DJI_GIMBAL_GROUP,		
		
	DJI_GROUP_SUM
} DjiMotorGroupEnum;


typedef struct Dji_Motor_Data_Struct_T
{
	uint8_t rx_flag;	// 接收标志
	
	float angle;		// 转子机械角度值范围: 0~8191 (对应转子机械角度为 0~360°）归1化后为0~1
	int16_t speed;          // 转子转速值的单位为：RPM
    float current;          // 控制电流范围—16384~0~+16384 归1化后为-1~1
    uint8_t temperature;    // 电机温度的单位为：℃
} DjiMotorDataTypeDef;


typedef struct Dji_Motor_Group_Struct_T
{
	CAN_HandleTypeDef *hcan;
	
	uint16_t TX_STD_ID[DJI_MOTOR_TX_SUM];
	uint16_t RX_STD_ID;
	
	DjiMotorDataTypeDef *motor_data;
	const uint8_t data_length;
	
} DjiMotorGroupTypeDef;

#pragma pack()


extern DjiMotorGroupTypeDef DJI_Motor[DJI_GROUP_SUM];


/* *************** Dji_motor Function *************** */
void Dji_Motor_Init(void);
void Dji_Motor_Chassis_Rx_Filter_Set(void);
char Dji_Motor_Get_Data(DjiMotorGroupEnum dji_group_x, CAN_RxHeaderTypeDef *rxheader, uint8_t *motor_rx_buffer);
void Dji_Motor_Data_Process(uint8_t *motor_rx_buff, DjiMotorGroupEnum dji_group_x, uint8_t motor_id);
void Dji_Motor_Set_Current(DjiMotorGroupEnum dji_group_x, DjiMotorTxIdEnum tx_id, int motor1, int motor2, int motor3, int motor4);



#endif

