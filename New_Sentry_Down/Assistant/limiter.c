#include "limiter.h"



/**
 * @brief 初始化角度变化率限制器
 * @param limiter 限制器指针
 * @param max_rate 最大变化率 (度/秒)
 */
void Angle_Rate_Limiter_Init(AngleRateLimitTypeDef *limiter, float max_rate)
{
    if (limiter == NULL) return;
    
    limiter->last_angle = 0.0f;
    limiter->max_rate = max_rate;
    limiter->output_angle = 0.0f;
}


/**
 * @brief 更新角度变化率限制器
 * @param limiter 限制器指针
 * @param input_angle 输入角度
 * @param dt 时间间隔 (秒)
 * @return 限制后的输出角度
 */
float Angle_Rate_Limiter_Update(AngleRateLimitTypeDef *limiter, float input_angle, float dt)
{
    if(limiter == NULL || dt <= 0.0f) {
        return input_angle;
    }

    // 计算允许的最大变化量
    float max_change = limiter->max_rate * dt;

    // 计算实际角度变化
    float angle_change = input_angle - limiter->last_angle;

    // 应用变化率限制
    float limited_change;
    if (fabsf(angle_change) > max_change) {
        limited_change = (angle_change > 0) ? max_change : -max_change;
    } else {
        limited_change = angle_change;
    }

    // 更新输出角度
    limiter->output_angle = limiter->last_angle + limited_change;

    // 保存当前角度用于下一次计算
    limiter->last_angle = limiter->output_angle;
    
    return limiter->output_angle;
}


/**
 * @brief 初始化角度限制配置
 * @param limit 限制配置指针
 * @param min_angle 最小角度
 * @param max_angle 最大角度
 * @param enabled 是否启用
 */
void Angle_Limit_Init(AngleLimitTypeDef *limit, float min_angle, float max_angle, float soft_limit, uint8_t enabled)
{
    if (limit == NULL) return;
    
    limit->min_angle = min_angle;
    limit->max_angle = max_angle;
    limit->soft_limit = soft_limit;
    limit->enabled = enabled;
}


/**
 * @brief 应用硬角度限制（直接截断）
 * @param input_angle 输入角度
 * @param limit 限制配置
 * @return 限制后的角度
 */
float Angle_Limit_Apply(float input_angle, const AngleLimitTypeDef *limit)
{
    if (limit == NULL || !limit->enabled) {
        return input_angle;
    }
    
    float limited_angle = input_angle;
    
    // 应用硬限制
    if (limited_angle < limit->min_angle) {
        limited_angle = limit->min_angle;
    } else if (limited_angle > limit->max_angle) {
        limited_angle = limit->max_angle;
    }
    
    return limited_angle;
}


/**
 * @brief 检查角度是否在限制范围内
 * @param limit 限制配置
 * @param angle 要检查的角度
 * @param error 指针：返回超出限制的误差
 * @return 0:在范围内，1:超出上界，-1:超出下界
 */
int8_t Angle_Limit_Check(const AngleLimitTypeDef *limit, float angle, float *error)
{
    if (limit == NULL || !limit->enabled) {
        if (error) *error = 0.0f;
        return 0;
    }
    
    if (angle < limit->min_angle) {
        if (error) *error = angle - limit->min_angle;  // 负值
        return -1;
    } else if (angle > limit->max_angle) {
        if (error) *error = angle - limit->max_angle;  // 正值
        return 1;
    } else {
        if (error) *error = 0.0f;
        return 0;
    }
}


/**
 * @brief 应用软角度限制（接近边界时减速）
 * @param input_angle 输入角度
 * @param limit 限制配置
 * @param current_angle 当前实际角度
 * @return 软限制后的角度
 */
float Angle_Limit_SoftLimit(float input_angle, const AngleLimitTypeDef *limit, float current_angle)
{
	if (limit == NULL || !limit->enabled) {
        return input_angle;
    }
	
	// 首先检查是否需要限制
    float limit_error;
    uint8_t limit_status = Angle_Limit_Check(limit, current_angle, &limit_error);
    
    // 如果当前角度已经超出限制，强制回到边界
    if (limit_status != 0) {
        return Angle_Limit_Apply(current_angle, limit);
    }
	
	// 检查是否接近边界
    float distance_to_min = current_angle - limit->min_angle;
    float distance_to_max = limit->max_angle - current_angle;
    
    // 判断移动方向是否朝向边界
    int moving_toward_min = (input_angle < current_angle);
    int moving_toward_max = (input_angle > current_angle);
    
    float limited_angle = input_angle;
    
    // 如果朝最小边界移动且接近边界
    if (moving_toward_min && distance_to_min < limit->soft_limit) {
        // 计算软限制因子：离边界越近，限制越强
        float soft_factor = distance_to_min / limit->soft_limit;
        if (soft_factor < 0.1f) soft_factor = 0.1f;  // 最小保留10%速度
        
        // 限制角度变化
        limited_angle = current_angle + (input_angle - current_angle) * soft_factor;
    }
    // 如果朝最大边界移动且接近边界
    else if (moving_toward_max && distance_to_max < limit->soft_limit) {
        // 计算软限制因子
        float soft_factor = distance_to_max / limit->soft_limit;
        if (soft_factor < 0.1f) soft_factor = 0.1f;
        
        // 限制角度变化
        limited_angle = current_angle + (input_angle - current_angle) * soft_factor;
    }
    
    // 最后应用硬限制确保不超出范围
    limited_angle = Angle_Limit_Apply(limited_angle, limit);
    
    return limited_angle;
}


/**
 * @brief 初始化速度斜坡控制器
 * @param ramp 斜坡控制器指针
 * @param max_speed 最大速度限制
 * @param max_acceleration 最大加速度
 * @param max_deceleration 最大减速度
 */
float Apply_Speed_Ramp(SpeedRampControlTypeDef *ramp, float target_speed, float dt)
{
    // 计算速度差值
    float speed_diff = target_speed - ramp->current_speed;

    // 判断是加速还是减速
    float max_accel = (speed_diff > 0) ? ramp->max_acceleration : ramp->max_deceleration;

    // 应用加速度限制
    float allowed_diff = copysignf(
        fminf(fabsf(speed_diff), max_accel * dt),
        speed_diff);

    // 更新当前速度
    ramp->current_speed += allowed_diff;

    // 应用速度限制88
    ramp->current_speed = CLAMP(ramp->current_speed, -ramp->max_speed, ramp->max_speed);

    return ramp->current_speed;
}

