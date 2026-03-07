#include "filter.h"





// ========================================= 一阶低通滤波器 ========================================= //

/**
  * @brief  一阶低通滤波器初始化
  * @param  filter: 滤波器结构体指针
  * @param  alpha: 滤波系数 (0-1)，越小滤波效果越强
  * @param  init_value: 初始值
  * @retval None
  */
void Lpf_Config(LpfFilterTypeDef *filter, float alpha, float init_value)
{
    filter->alpha = alpha;
    filter->prev_value = init_value;
    filter->time_stamp = 0;
}


/**
  * @brief  一阶低通滤波器更新（固定时间间隔）
  * @param  filter: 滤波器结构体指针
  * @param  input: 输入值
  * @retval 滤波后的输出值
  */
float Lpf_Update(LpfFilterTypeDef *filter, float input)
{
	  filter->prev_value = filter->alpha * input + (1 - filter->alpha) * filter->prev_value;
    return filter->prev_value;
}


/**
  * @brief  一阶低通滤波器更新（可变时间间隔）
  * @param  filter: 滤波器结构体指针
  * @param  input: 输入值
  * @param  dt: 时间间隔（秒）
  * @retval 滤波后的输出值
  */
float LPF_Update_Dt(LpfFilterTypeDef *filter, float input, float dt)
{
    // 根据时间间隔计算动态alpha
    float alpha = dt / (filter->alpha + dt);
    filter->prev_value = alpha * input + (1 - alpha) * filter->prev_value;
    return filter->prev_value;
}


// ========================================= 滑动平均滤波器 ========================================= //

/**
  * @brief  滑动平均滤波器初始化
  * @param  filter: 滤波器结构体指针
  * @param  buffer: 数据缓冲区
  * @param  size: 缓冲区大小
  * @retval None
  */
void Mov_Config(MovFilterTypeDef *filter, float *buffer, uint16_t size)
{
    filter->buffer = buffer;
    filter->size = size;
    filter->index = 0;
    filter->sum = 0;
    filter->is_full = 0;

    // 初始化缓冲区
    memset(filter->buffer, 0, size * sizeof(float));
}


/**
  * @brief  滑动平均滤波器更新
  * @param  filter: 滤波器结构体指针
  * @param  input: 输入值
  * @retval 滤波后的输出值
  */
float Mov_Update(MovFilterTypeDef *filter, float input)
{
	// 减去即将被覆盖的值
    if(filter->is_full) {
      filter->sum -= filter->buffer[filter->index];
    }
	
	// 添加新值
    filter->buffer[filter->index] = input;
    filter->sum += input;
	
	// 更新索引
    filter->index++;
    if (filter->index >= filter->size) {
        filter->index = 0;
        filter->is_full = 1;
    }
	
	// 计算平均值
    uint16_t count = filter->is_full ? filter->size : filter->index;
    return filter->sum / count;
}


/**
  * @brief  清空滑动平均滤波器
  * @param  filter: 滤波器结构体指针
  * @retval None
  */
void Mov_Clear(MovFilterTypeDef *filter)
{
    filter->index = 0;
    filter->sum = 0;
    filter->is_full = 0;

    memset(filter->buffer, 0, filter->size * sizeof(float));
}


// ========================================= 中值滤波器 ========================================= //

/**
  * @brief  中值滤波器初始化
  * @param  filter: 滤波器结构体指针
  * @param  buffer: 数据缓冲区
  * @param  size: 缓冲区大小
  * @retval None
  */
void Med_Config(MedianFilterTypeDef *filter, float *buffer, uint16_t size)
{
    filter->buffer = buffer;
    filter->size = size;
    filter->index = 0;
    filter->is_full = 0;

    // 初始化缓冲区
    memset(filter->buffer, 0, size * sizeof(float));
}


/**
  * @brief  冒泡排序（内部函数）
  * @param  arr: 待排序数组
  * @param  size: 数组大小
  * @retval None
  */
static void Bubble_Sort(float *arr, uint16_t size)
{
    uint16_t i, j;
    float temp;
    
    for (i = 0; i < size - 1; i++) {
        for (j = 0; j < size - 1 - i; j++) {
            if (arr[j] > arr[j + 1]) {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}


/**
  * @brief  中值滤波器更新
  * @param  filter: 滤波器结构体指针
  * @param  input: 输入值
  * @retval 滤波后的输出值
  */
float Med_Update(MedianFilterTypeDef *filter, float input)
{
    // 更新缓冲区
    filter->buffer[filter->index] = input;
    filter->index++;
    if (filter->index >= filter->size) {
        filter->index = 0;
        filter->is_full = 1;
    }
    
    // 复制缓冲区用于排序
    uint16_t count = filter->is_full ? filter->size : filter->index;
    float temp_buffer[filter->size];
    memcpy(temp_buffer, filter->buffer, count * sizeof(float));
    
    // 排序并返回中值
    Bubble_Sort(temp_buffer, count);
    return temp_buffer[count / 2];
}


// ========================================= IIR滤波器 ========================================= //

/**
  * @brief  IIR滤波器初始化
  * @param  filter: 滤波器结构体指针
  * @param  a_coeff: 分母系数数组
  * @param  b_coeff: 分子系数数组
  * @param  x_buffer: 输入历史缓冲区
  * @param  y_buffer: 输出历史缓冲区
  * @param  order: 滤波器阶数
  * @retval None
  */
void IIR_Config(IIRFilterTypeDef *filter, float *a_coeff, float *b_coeff, float *x_buffer, float *y_buffer, uint16_t order)
{
    filter->a_coeff = a_coeff;
    filter->b_coeff = b_coeff;
    filter->x_buffer = x_buffer;
    filter->y_buffer = y_buffer;
    filter->order = order;
    filter->index = 0;
    
    // 清空缓冲区
    memset(x_buffer, 0, (order + 1) * sizeof(float));
    memset(y_buffer, 0, (order + 1) * sizeof(float));
}


/**
  * @brief  IIR滤波器更新
  * @param  filter: 滤波器结构体指针
  * @param  input: 输入值
  * @retval 滤波后的输出值
  */
float IIR_Update(IIRFilterTypeDef *filter, float input)
{
    uint8_t i, j;
    float output = 0.0f;
    
    // 更新输入缓冲区
    for (i = filter->order; i > 0; i--) {
        filter->x_buffer[i] = filter->x_buffer[i - 1];
    }
    filter->x_buffer[0] = input;

  // 更新输出缓冲区
    for (i = filter->order; i > 0; i--) {
        filter->y_buffer[i] = filter->y_buffer[i - 1];
    }

  // 计算输出（直接II型）
    output = filter->b_coeff[0] * filter->x_buffer[0];
    for (j = 1; j <= filter->order; j++) {
        output += filter->b_coeff[j] * filter->x_buffer[j] - filter->a_coeff[j] * filter->y_buffer[j];
    }
    output /= filter->a_coeff[0];
    
    filter->y_buffer[0] = output;
    return output;
}


// ========================================= 限幅滤波器 ========================================= //

/**
  * @brief  限幅滤波器
  * @param  input: 输入值
  * @param  min_val: 最小值
  * @param  max_val: 最大值
  * @retval 限幅后的输出值
  */
float Limiter_Filter(float input, float min_val, float max_val)
{
    if (input > max_val) return max_val;
    if (input < min_val) return min_val;
    return input;
}


// ========================================= 死区滤波器 ========================================= //

/**
  * @brief  死区滤波器
  * @param  input: 输入值
  * @param  dead_zone: 死区大小
  * @retval 滤波后的输出值
  */
float DeadZone_Filter(float input, float dead_zone)
{
    if (fabsf(input) <= dead_zone) {
        return 0.0f;
    }
    return input;
}


// ========================================= 斜坡滤波器 ========================================= //

/**
  * @brief  斜坡滤波器（限制变化率）
  * @param  input: 输入值
  * @param  prev_output: 上一次输出值
  * @param  max_change: 最大变化量
  * @retval 斜坡处理后的输出值
  */
float Ramp_Filter(float input, float prev_output, float max_change)
{
    float change = input - prev_output;
    
    if (change > max_change) {
        return prev_output + max_change;
    } else if (change < -max_change) {
        return prev_output - max_change;
    } else {
        return input;
    }
}

