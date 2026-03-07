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



// 归一函数，将数据归一化到[-1,1]区间
#define NORMALIZE(x, min, max) (((2.0 * ((x) - (min))) / ((max) - (min))) - 1.0)


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


/**
 * @brief 计算校验和
 * @param data 数据指针
 * @param len 数据长度
 * @return 校验和
 */
static uint8_t calculate_checksum(uint8_t *data, uint8_t len) {
    uint8_t sum = 0;
    for (uint8_t i = 0; i < len; i++) {
        sum += data[i];
    }
    return sum;
}


#endif

