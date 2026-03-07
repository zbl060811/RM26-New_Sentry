#include "curve_mapping.h"



/**
 * @brief 线性映射（y = x * scale）
 * @param input 输入值 [-1, 1]
 * @param scale 缩放比例
 * @return 输出值 [-scale, scale]
 */
float Curve_Linear(float input, float scale)
{
    // 输入限幅
    if(input > 1.0f) input = 1.0f;
    if(input < -1.0f) input = -1.0f;
    
    return input * scale;
}


/**
 * @brief 平方曲线（y = sign(x) * x2 * scale）
 * @param input 输入值 [-1, 1]
 * @param scale 缩放比例
 * @return 输出值 [-scale, scale]
 */
float Curve_Quadratic(float input, float scale)
{
    // 输入限幅
    if(input > 1.0f) input = 1.0f;
    if(input < -1.0f) input = -1.0f;
    
    float sign = (input >= 0) ? 1.0f : -1.0f;
    return sign * input * input * scale;
}


/**
    * @brief 立方曲线（y = x3 * scale）
 * @param input 输入值 [-1, 1]
 * @param scale 缩放比例
 * @return 输出值 [-scale, scale]
 */
float Curve_Cubic(float input, float scale)
{
    // 输入限幅
    if(input > 1.0f) input = 1.0f;
    if(input < -1.0f) input = -1.0f;
    
    return input * input * input * scale;
}


/**
 * @brief 平方根曲线（y = sign(x) * sqrt(abs(x)) * scale）
 * @param input 输入值 [-1, 1]
 * @param scale 缩放比例
 * @return 输出值 [-scale, scale]
 */
float Curve_SquareRoot(float input, float scale)
{
    // 输入限幅
    if(input > 1.0f) input = 1.0f;
    if(input < -1.0f) input = -1.0f;
    
    float sign = (input >= 0) ? 1.0f : -1.0f;
    return sign * sqrtf(fabsf(input)) * scale;
}


/**
 * @brief 指数曲线（y = sign(x) * |x|^exp * scale） 
 * @param input 输入值 [-1, 1]
 * @param exp 指数
 * @param scale 缩放比例
 * @return 输出值 [-scale, scale]
 */
float Curve_Exponential(float input, float exp, float scale)
{
    // 输入限幅
    if(input > 1.0f) input = 1.0f;
    if(input < -1.0f) input = -1.0f;
    
    float sign = (input >= 0) ? 1.0f : -1.0f;
    return sign * powf(fabsf(input), exp) * scale;
}


/** 
 * @brief S曲线（三次平滑）
 * @param input 输入值 [-1, 1]
 * @param scale 缩放比例
 * @return 输出值 [-scale, scale]
 */
float Curve_SCurve(float input, float scale)
{
    // 输入限幅
    if(input > 1.0f) input = 1.0f;
    if(input < -1.0f) input = -1.0f;
    
    // 三次S曲线：y = 3x? - 2x?
    float result;
    if(input >= 0) {
        result = 3.0f * input * input - 2.0f * input * input * input;
    } else {
        result = -3.0f * input * input - 2.0f * input * input * input;
    }
    
    return result * scale;
}


/** 
 * @brief 死区+线性
 * @param input 输入值 [-1, 1]
 * @param deadzone 死区范围 [0, 1)
 * @param scale 缩放比例
 * @return 输出值 [-scale, scale]
 */
float Curve_DeadzoneLinear(float input, float deadzone, float scale)
{
    // 死区处理
    if(fabsf(input) < deadzone) {
        return 0.0f;
    }
    
    // 重新映射到[0,1]
    float mapped;
    if(input > 0) {
        mapped = (input - deadzone) / (1.0f - deadzone);
    } else {
        mapped = (input + deadzone) / (1.0f - deadzone);
    }
    
    // 限幅
    if(mapped > 1.0f) mapped = 1.0f;
    if(mapped < -1.0f) mapped = -1.0f;
    
    return mapped * scale;
}


/** 
 * @brief expo曲线（RC遥控器常用）
 * @param input 输入值 [-1, 1]
 * @param expo 指数系数 (0:线性, >0:曲线)
 * @param rate 最大输出比例
 * @return 输出值 [-rate, rate]
 */
float Curve_Expo(float input, float expo, float rate)
{
    // 输入限幅
    if(input > 1.0f) input = 1.0f;
    if(input < -1.0f) input = -1.0f;
    
    // Expo公式：output = input * (1 - expo + expo * input?)
    float result = input * (1.0f - expo + expo * input * input);
    
    return result * rate;
}


/** 
 * @brief 自定义查表曲线
 * @param input 输入值 [-1, 1]
 * @param table 查找表指针
 * @return 输出值
 */
float Curve_TableLookup(float input, const CurveTableTypeDef *table)
{
    if(table == NULL || table->point_count < 2) {
        return input;
    }
    
    // 输入限幅
    if(input < -1.0f) input = -1.0f;
    if(input > 1.0f) input = 1.0f;
    
    // 处理负输入（假设表对称）
    uint8_t is_negative = 0;
    if(input < 0) {
        is_negative = 1;
        input = -input;
    }
    
    // 查找对应的区间
    for(uint16_t i = 0; i < table->point_count - 1; i++) {
        if(input >= table->input_points[i] && input <= table->input_points[i+1]) {
            // 线性插值
            float t = (input - table->input_points[i]) / 
                     (table->input_points[i+1] - table->input_points[i]);
            float output = table->output_points[i] + 
                          t * (table->output_points[i+1] - table->output_points[i]);
            
            return is_negative ? -output : output;
        }
    }
    
    // 如果没找到，返回边界值
    float output = (input <= table->input_points[0]) ? 
                   table->output_points[0] : 
                   table->output_points[table->point_count-1];
    
    return is_negative ? -output : output;
}


/**
 * @brief 初始化曲线配置结构体
 * @param config 曲线配置指针
 * @param type 曲线类型
 * @param param1 参数1（指数/死区等）
 * @param param2 参数2（增益/系数等）
 * @param output_scale 输出缩放比例
 * @param deadzone 死区大小
 */
void Curve_Config_Init(CurveConfigTypeDef *config, 
                       CurveTypeEnum type, 
                       float param1, 
                       float param2, 
                       float output_scale,
                       float deadzone)
{
    if(config == NULL) return;
    
    config->type = type;
    config->param1 = param1;
    config->param2 = param2;
    config->output_scale = output_scale;
    config->deadzone = deadzone;
}

 
/**
 * @brief 通用曲线映射接口
 * @param input 输入值 [-1, 1]
 * @param config 曲线配置指针
 * @return 输出值
 */
float Curve_Map(float input, const CurveConfigTypeDef *config)
{
    if(config == NULL) {
        return input;
    }
    
    float result = 0.0f;
    
    switch(config->type) {
        case CURVE_LINEAR: result = Curve_Linear(input, config->output_scale); break;
        case CURVE_QUADRATIC: result = Curve_Quadratic(input, config->output_scale); break;
        case CURVE_CUBIC: result = Curve_Cubic(input, config->output_scale); break;
        case CURVE_SQUARE_ROOT: result = Curve_SquareRoot(input, config->output_scale); break;
        case CURVE_EXPONENTIAL: result = Curve_Exponential(input, config->param1, config->output_scale); break;
        case CURVE_S_CURVE: result = Curve_SCurve(input, config->output_scale); break;
        case CURVE_DEADZONE_LINEAR: result = Curve_DeadzoneLinear(input, config->deadzone, config->output_scale); break;
        case CURVE_EXPO: result = Curve_Expo(input, config->param1, config->param2); break;
        case CURVE_CUSTOM_TABLE:
            // 需要外部提供表
            result = input;
            break;
        default:
            result = input;
            break;
    }
    
    return result;
}


/**
 * @brief 带死区处理的完整映射
 * @param input 输入值 [-1, 1]
 * @param config 曲线配置指针
 * @return 输出值
 */
float Curve_MapWithDeadzone(float input, const CurveConfigTypeDef *config)
{
    if(config == NULL) {
        return input;
    }
    
    // 先应用死区（如果配置有死区）
    float processed_input = input;
    if(config->deadzone > 0.0f) {
        if(fabsf(input) < config->deadzone) {
            processed_input = 0.0f;
        }
    }
    
    // 再应用曲线映射
    return Curve_Map(processed_input, config);
}


/** 
 * @brief 创建RC摇杆曲线配置（常用配置）
 * @param expo Expo系数 (0.0-1.0)
 * @param rate 输出比例 (0.0-1.0)
 * @param deadzone 死区大小 (0.0-1.0)
 * @return 曲线配置结构体
 */
CurveConfigTypeDef Curve_CreateRCStickConfig(float expo, float rate, float deadzone)
{
    CurveConfigTypeDef config;
    
    config.type = CURVE_EXPO;
    config.param1 = expo;           // Expo系数
    config.param2 = rate;           // 输出比例
    config.output_scale = 1.0f;
    config.deadzone = deadzone;
    
    return config;
}







