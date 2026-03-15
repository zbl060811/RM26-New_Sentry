#include "mg_motor.h"


MGMotorTypeDef mg_motor;



void Mg_Motor_Init(MGMotorTypeDef *Motor)
{
    Motor->params.hcan = &hcan2; // 设置CAN句柄
    Motor->params.motor_id = MG_Motor_ID_1; // 设置电机ID
    Motor->params.angle = 10000; // 初始角度
    Motor->params.speed = 500; // 初始速度

    Motor->speed_max = 500; // 最大输出速度（单位：0.01度/秒）
    Motor->angle_min = -20000; // 最小输出角度（单位：0.01度）
    Motor->angle_max = 20000; // 最大输出角度（单位：0.01度）

    MG_Motor_Run(&Motor->params); // 启动电机
    MG_Motor_PositionControl(Motor); // 发送初始位置控制命令
}


// 内部通用发送函数
/// @brief 发送MG电机命令
/// @param params 电机控制参数结构体指针
/// @param cmd 命令类型
/// @param data 数据缓冲区指针
/// @param data_len 数据长度
/// @return 返回HAL_StatusTypeDef类型的操作状态，表示CAN消息发送是否成功
HAL_StatusTypeDef MG_Motor_SendCommand(MotorControlParamsTypeDef *params,
                                              const uint8_t *data,
                                              uint8_t data_len)
{
    CAN_HandleTypeDef *hcan = params->hcan;

    CAN_TxHeaderTypeDef tx_header =
        {
            .StdId = params->motor_id,
            .RTR = CAN_RTR_DATA,
            .IDE = CAN_ID_STD,
            .DLC = data_len};

    return HAL_CAN_AddTxMessage(hcan, &tx_header, (uint8_t *)data, NULL);
}


/// @brief 关闭电机
/// @param params 电机控制参数
/// @return HAL操作状态
HAL_StatusTypeDef MG_Motor_Shutdown(MotorControlParamsTypeDef *params)
{
    uint8_t data[8] = {MG_CLOSER_CMD};
    return MG_Motor_SendCommand(params, data, sizeof(data));
}


/// @brief 停止电机
/// @param params 电机控制参数
/// @return HAL操作状态
HAL_StatusTypeDef MG_Motor_Stop(MotorControlParamsTypeDef *params)
{
    uint8_t data[8] = {MG_STOP_CMD};
    return MG_Motor_SendCommand(params, data, sizeof(data));
}


/// @brief 启动电机
/// @param params 电机控制参数
/// @return HAL操作状态
HAL_StatusTypeDef MG_Motor_Run(MotorControlParamsTypeDef *params)
{
    uint8_t data[8] = {MG_OPEN_CMD};
    return MG_Motor_SendCommand(params, data, sizeof(data));
}


/// @brief 发送多圈位置控制命令
/// @param params 电机控制参数
/// @param Motor 电机全局配置结构体指针
/// @return HAL操作状态
HAL_StatusTypeDef MG_Motor_PositionControl(MGMotorTypeDef *Motor)
{
    uint8_t data[8] = {MG_MULTI_TURN_POSITION_CLOSE_2_CMD};

    // 设置最大速度
    if(Motor->params.speed > Motor->speed_max) {
        Motor->params.speed = Motor->speed_max;
    }

    // 限制角度范围
    if (Motor->params.angle > Motor->angle_max)
    {
        Motor->params.angle = Motor->angle_max;
    }
    else if (Motor->params.angle < Motor->angle_min)
    {
        Motor->params.angle = Motor->angle_min;
    }

    // 打包速度参数
    data[2] = (uint8_t)(Motor->params.speed & 0xFF);
    data[3] = (uint8_t)((Motor->params.speed >> 8) & 0xFF);

    // 打包角度参数
    data[4] = (uint8_t)(Motor->params.angle & 0xFF);
    data[5] = (uint8_t)((Motor->params.angle >> 8) & 0xFF);
    data[6] = (uint8_t)((Motor->params.angle >> 16) & 0xFF);
    data[7] = (uint8_t)((Motor->params.angle >> 24) & 0xFF);

    return MG_Motor_SendCommand(&Motor->params, data, sizeof(data));
}



