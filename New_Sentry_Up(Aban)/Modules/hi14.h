#ifndef __HI14_H__
#define __HI14_H__


#include "bsp_usart.h"
#include "rm_typedef.h"


#define RX_BUFFER_SIZE		256		// DMA缓冲区大小
#define SOF_FRAME_1			0X5A	// 帧头同步标志第一帧
#define SOF_FRAME_2			0XA5	// 帧头同步标志第二帧
#define DATA_FIELD_SIZE		76		// 数据域长度
#define DATA_PACKET_TAG		0X91	// 数据包标签

#define HI14_TICK_TIME		10
#define HI14_USE_USART		huart6
#define	HI14_USE_USART_DMA_RX	hdma_usart6_rx


#pragma pack(1)

typedef enum Hi_MainStatus_Flag_Enum_T
{
	// 位3：零偏收敛状态报警
    STATUS_WB_CONV        = (1u << 3),
    // 位4：磁场异常检测报警  
    STATUS_MAG_DIST       = (1u << 4),
    // 位10：磁参与姿态解算标志
    STATUS_MAG_AIDING     = (1u << 10),
    // 位11：时间同步标志
    STATUS_UTC_TIME       = (1u << 11),
    // 位12：SOUT脉冲输出标志
    STATUS_SOUT_PULSE     = (1u << 12),
} HiMainStatusFlagEnum;

typedef enum Hi_StatusValue_Enum_T
{
	BIAS_STATUS_GOOD = 0,      // 零偏收敛良好
    BIAS_STATUS_POOR,          // 零偏收敛精度差
    
    MAG_ENV_GOOD = 0,          // 地磁环境良好
    MAG_ENV_DISTURBED,         // 磁场干扰
    
    MAG_MODE_6AXIS = 0,        // 6轴模式
    MAG_MODE_9AXIS,            // 9轴模式（磁参与）
    
    TIME_LOCAL = 0,            // 本地时间
    TIME_UTC_SYNCED,           // UTC时间同步
    
    SOUT_NO_PULSE = 0,         // 无SOUT脉冲
    SOUT_PULSE_OUTPUT,         // SOUT脉冲输出
} HiStatusValueEnum;


typedef struct Hi_Acceleration_Struct_T
{
	// 加速度(mg)
	float ax;	
	float ay;	
	float az;
} HiAccelerationTypeDef;

typedef struct Hi_Gryo_Struct_T
{
	// 角速度(deg/s)
	float gx;
	float gy;
	float gz;
} HiGryoTypeDef;

typedef struct Hi_Magnet_Struct_T
{
	// 磁强度(uT)
	float mx;
	float my;
	float mz;
} HiMagnetTypeDef;
	
typedef struct Hi_Euler_Angle_Struct_T
{
	// 欧拉角(度)
	float roll;
	float pitch;
	float yaw;
} HiEulerAngleTypeDef;

typedef struct Hi_Auaternion_Struct_T
{
	// 四元数 (W,X,Y,Z)
	float qw;
	float qx;
	float qy;
	float qz;
} HiAuaternionTypeDef;

typedef struct Hi_IMU_Status_Struct_T
{
	// 原始状态字
	uint16_t raw_status;
	
	// 解析后的状态信息
	struct{
		HiStatusValueEnum bias_convergence;    // 零偏收敛状态
        HiStatusValueEnum mag_environment;     // 磁场环境
        HiStatusValueEnum mag_mode;            // 磁力计模式
        HiStatusValueEnum time_sync;           // 时间同步状态
        HiStatusValueEnum sout_pulse;          // SOUT脉冲状态
	} parsed;
	
	struct{
		bool is_healthy;           // 总体健康状态
        bool needs_calibration;    // 需要校准
        bool has_mag_interference; // 存在磁场干扰
        bool time_synced;          // 时间已同步
	} quality;
	
} HiIMUStatusTypeDef;

typedef struct Hi_Data_Struct_T
{
	uint8_t tag;			// 数据包标签
	uint16_t main_status;	// 状态字
	int8_t temperature;		// 模块平均温度
	float air_pressure;		// 气压
	uint32_t system_time;	// UTC时间,暂未使用
	
	HiAccelerationTypeDef acc;	 // 加速度XYZ
	HiGryoTypeDef gyro;		// 角速度XYZ
	HiMagnetTypeDef magent;		 // 磁强度XYZ
	HiEulerAngleTypeDef euler;	 // 欧拉角
	HiAuaternionTypeDef quater;	 // 节点四元数WXYZ
} HiDataTypeDef;

typedef struct Hi_Struct_T
{
	uint8_t flag;
	uint32_t tick;
	uint8_t rx_buffer[RX_BUFFER_SIZE];	// 接收缓冲区
	
	HiIMUStatusTypeDef hi_status;	// 超核状态
	HiDataTypeDef hi_data;		// 超核数据
} HiTypeDef;



#pragma pack()


extern HiTypeDef Hi14;

/* *************** Hi14 Function *************** */
void Hi14_Init(void);
void Crc16_Update(uint16_t *currentCrc, const uint8_t *src, uint32_t lengthInBytes);
void Hi14_Timing_Callback(void);
uint16_t Hi_Parse_Data(uint8_t *data, uint16_t size);
void Hi14_Parse_IMUFrame(const HiDataTypeDef* frame);
void Hi14_Check_Status(uint16_t main_status, HiIMUStatusTypeDef* status);
void Hi14_Rx_Callback(uint16_t size);
void Hi14_Send_Cmd(char *cmd);
void Hi14_Set_Reset(void);
void Hi14_Set_Save(void);
void Hi14_Set_Baud(uint32_t baudrate);







#endif
