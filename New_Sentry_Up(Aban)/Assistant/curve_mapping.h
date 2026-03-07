#ifndef __CURVE_MAPPING_H__
#define __CURVE_MAPPING_H__



#include "rm_typedef.h"



#pragma pack(1)



// 曲线类型枚举
typedef enum Curve_Type_Enum_T
{
    CURVE_LINEAR = 0,     // 线性
    CURVE_QUADRATIC,      // 平方
    CURVE_CUBIC,          // 立方
    CURVE_CUBIC_DEAD,     // 立方+死区
    CURVE_SQUARE_ROOT,    // 平方根
    CURVE_EXPONENTIAL,    // 指数
    CURVE_S_CURVE,        // S曲线
    CURVE_DEADZONE_LINEAR,// 死区+线性
    CURVE_EXPO,           // Expo曲线（RC常用）
    CURVE_CUSTOM_TABLE    // 自定义查表
} CurveTypeEnum;


// 曲线配置结构体
typedef struct Curve_Config_Struct_T
{
    CurveTypeEnum type;      // 曲线类型
    float param1;            // 参数1（指数/死区等）
    float param2;            // 参数2（增益/系数等）
    float output_scale;      // 输出缩放
    float deadzone;          // 死区大小
} CurveConfigTypeDef;


// 自定义曲线查表结构
typedef struct Curve_Table_Struct_T
{
    uint16_t point_count;    // 点数
    float input_points[16];  // 输入点
    float output_points[16]; // 输出点
} CurveTableTypeDef;


#pragma pack()



/* *************** 基础曲线函数 *************** */

/**
 * @brief 线性映射（y = x * scale）
 */
float Curve_Linear(float input, float scale);

/**
 * @brief 平方曲线（y = sign(x) * x2 * scale）
 */
float Curve_Quadratic(float input, float scale);

/**
 * @brief 立方曲线（y = x3 * scale）
 */
float Curve_Cubic(float input, float scale);

/**
 * @brief 平方根曲线（y = sign(x) * sqrt(|x|) * scale）
 */
float Curve_SquareRoot(float input, float scale);

/**
 * @brief 指数曲线（y = sign(x) * |x|^exp * scale）
 */
float Curve_Exponential(float input, float exp, float scale);

/**
 * @brief S曲线（三次平滑）
 */
float Curve_SCurve(float input, float scale);

/**
 * @brief 死区+线性
 */
float Curve_DeadzoneLinear(float input, float deadzone, float scale);

/**
 * @brief Expo曲线（RC遥控器常用）
 * @param input 输入值 [-1, 1]
 * @param expo 指数系数 (0:线性, >0:曲线)
 * @param rate 最大输出比例
 */
float Curve_Expo(float input, float expo, float rate);

/**
 * @brief 自定义查表曲线
 */
float Curve_TableLookup(float input, const CurveTableTypeDef *table);



/* *************** 高级封装函数 *************** */

/**
 * @brief 通用曲线映射（根据配置）
 */
float Curve_Map(float input, const CurveConfigTypeDef *config);

/**
 * @brief 初始化曲线配置
 */
void Curve_Config_Init(CurveConfigTypeDef *config, 
                       CurveTypeEnum type, 
                       float param1, 
                       float param2, 
                       float output_scale,
                       float deadzone);

/**
 * @brief 带死区处理的完整映射
 */
float Curve_MapWithDeadzone(float input, const CurveConfigTypeDef *config);

/**
 * @brief 创建RC摇杆曲线（常用配置）
 * @param expo Expo系数 (0.0-1.0)
 * @param rate 输出比例 (0.0-1.0)
 * @param deadzone 死区大小
 */
CurveConfigTypeDef Curve_CreateRCStickConfig(float expo, float rate, float deadzone);


#endif



