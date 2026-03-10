#ifndef __RM_TYPEDEF_H__
#define __RM_TYPEDEF_H__



#include "stm32f4xx_hal.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>



#define PI      3.1415926535897932384626433832795f

#define CLAMP(x, min, max)  ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))



/**
 * @brief 将数值归一化到[-1, 1]范围，并限制在范围内
 * @param x 输入值
 * @param min 输入最小值
 * @param max 输入最大值
 * @return 归一化后的值，范围限制在[-1, 1]
 */
static inline float normalize(float x, float min, float max)
{
    float range = max - min;
    if (fabsf(range) < 1e-6f) {  // 避免除零
        return 0.0f;
    }
    
    float normalized = ((2.0f * (x - min)) / range) - 1.0f;
    
    // 限制在[-1, 1]范围内
    if (normalized > 1.0f) return 1.0f;
    if (normalized < -1.0f) return -1.0f;
    return normalized;
}


/**
 * @brief 映射函数
 * @param x 输入值
 * @param in_min 输入最小值
 * @param in_max 输入最大值
 * @param out_min 输出最小值
 * @param out_max 输出最大值
 * @return 映射后的值
 */
static inline double msp(double x, double in_min, double in_max, double out_min, double out_max) 
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}





#endif

