#ifndef __DJI_MOTOR_H__
#define __DJI_MOTOR_H__



#include "rm_typedef.h"
#include "bsp_can.h"


#pragma pack(1)


typedef enum Dji_Motor_Tx_ID_Enum_T
{
	DJI_MOTOR_TX_1_4,
	DJI_MOTOR_TX_5_8,
	DJI_MOTOR_TX_SUM
} DjiMotorTxIdEnum;

typedef enum Dji_Motor_Gimbal_Group_Enum_T
{
	DJI_MOTOR_GIMBAL_GM6020_YAW_RX_1,		// 云台上yaw轴电机
	DJI_MOTOR_GIMBAL_RX_SUM
} DjiMotorGimbalGroupEnum;

typedef enum Dji_Motor_Shoot_Group_Enum_T
{
	DJI_MOTOR_SHOOT_LEFT_M2006_RX_1,		// 发射机构左摩擦轮
	DJI_MOTOR_SHOOT_RIGHT_M2006_RX_2,		// 发射机构右摩擦轮
	DJI_MOTOR_SHOOT_FEED_M2006_RX_3,		// 拨盘供弹电机
	DJI_MOTOR_SHOOT_RX_SUM
} DjiMotorShootGroupEnum;


typedef enum Dji_Motor_Group_Enum_T
{  
	/*
				  腰盘组
	   云台上yaw 
	*/
	DJI_GIMBAL_GROUP,		
	
	/*
				  发射组
	   左摩擦轮+右摩擦轮+供弹拨盘
	*/
	DJI_SHOOT_GROUP,
		
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
void Dji_Motor_Rx_Filter_Set(void);
char Dji_Motor_Get_Data(DjiMotorGroupEnum dji_group_x, CAN_RxHeaderTypeDef *rxheader, uint8_t *motor_rx_buffer);
void Dji_Motor_Data_Process(uint8_t *motor_rx_buff, DjiMotorGroupEnum dji_group_x, uint8_t motor_id);
void Dji_Motor_Set_Current(DjiMotorGroupEnum dji_group_x, DjiMotorTxIdEnum tx_id, int motor1, int motor2, int motor3, int motor4);



#endif

