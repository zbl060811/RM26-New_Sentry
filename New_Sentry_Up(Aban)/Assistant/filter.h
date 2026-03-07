#ifndef __FILTER_H__
#define __FILTER_H__


#include "rm_typedef.h"



#pragma pack(1)

/* 滤波器类型 */
typedef enum Filter_Type_Enum_T	
{
	FILTER_NONE = 0,	// 无滤波
	FILTER_LPF_FIRST_ORDER,		// 一阶低通滤波
	FILTER_MOV_AVER,	// 滑动平均滤波
	FILTER_MEDIAN,		// 中值滤波
	FILTER_IIR,			// IIR滤波
	FILTER_LIMITER,		// 限幅滤波
	FILTER_DEAD,		// 死区滤波
	FILTER_RAMP			// 斜坡滤波
} FilterTypeEnum;


/* 一阶低通滤波器 */
typedef struct Lpf_Filter_Struct_T
{
	float alpha;		// 系数（0-1）
	float prev_value;	// 先前值
	uint32_t time_stamp;	// 时间戳
} LpfFilterTypeDef;	


/* 滑动平均滤波器 */
typedef struct Mov_Filter_Struct_T
{
	float *buffer;          // 数据缓冲区
    uint16_t size;          // 缓冲区大小
    uint16_t index;         // 当前索引
    float sum;              // 数据总和
    uint8_t is_full;        // 缓冲区是否已满
} MovFilterTypeDef;


/* 中值滤波器结构体 */
typedef struct Median_Filter_Struct_T
{
	float *buffer;          // 数据缓冲区
    uint16_t size;          // 缓冲区大小
    uint16_t index;         // 当前索引
    uint8_t is_full;        // 缓冲区是否已满
} MedianFilterTypeDef;


/* IIR滤波器结构体 */
typedef struct IIR_Filter_Struct_T
{
    float *a_coeff;         // 分母系数 (a0, a1, a2...)
    float *b_coeff;         // 分子系数 (b0, b1, b2...)
    float *x_buffer;        // 输入历史缓冲区
    float *y_buffer;        // 输出历史缓冲区
    uint8_t order;          // 滤波器阶数
    uint8_t index;          // 当前索引
} IIRFilterTypeDef;


/* 限幅滤波器结构体 */
typedef struct Limiter_Filter_Struct_T
{
	float max_value;	// 最大值
	float min_value;	// 最小值
} LimiterFilterTypeDef;


/* 死区滤波器结构体 */
typedef struct Dead_Zone_Filter_Struct_T
{
	float dead_zone;	// 死区值
} DeadZoneFilterTypeDef;


/* 斜坡滤波器结构体 */
typedef struct Ramp_Filter_Struct_T
{
	float change;		// 变化率
} RampFilterTypeDef;




typedef struct Filter_Struct_T
{
	FilterTypeEnum type;	// 滤波器类型
	union{
		LpfFilterTypeDef lpf;
		MovFilterTypeDef mov;
		MedianFilterTypeDef med;
		IIRFilterTypeDef iir;
		LimiterFilterTypeDef lim;
		DeadZoneFilterTypeDef dead;
		RampFilterTypeDef ramp;
	} filter_type;
	
} FilterTypeDef;


#pragma pack()



/* *************** Filter Function *************** */
void Lpf_Config(LpfFilterTypeDef *filter, float alpha, float init_value);
float Lpf_Update(LpfFilterTypeDef *filter, float input);
float LPF_Update_Dt(LpfFilterTypeDef *filter, float input, float dt);

void Mov_Config(MovFilterTypeDef *filter, float *buffer, uint16_t size);
float Mov_Update(MovFilterTypeDef *filter, float input);
void Mov_Clear(MovFilterTypeDef *filter);

void Med_Config(MedianFilterTypeDef *filter, float *buffer, uint16_t size);
float Med_Update(MedianFilterTypeDef *filter, float input);

void IIR_Config(IIRFilterTypeDef *filter, float *a_coeff, float *b_coeff, float *x_buffer, float *y_buffer, uint16_t order);
float IIR_Update(IIRFilterTypeDef *filter, float input);

float Limiter_Filter(float input, float min_val, float max_val);

float DeadZone_Filter(float input, float dead_zone);

float Ramp_Filter(float input, float prev_output, float max_change);







#endif

