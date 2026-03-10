#include "vofa.h"

#include "chassis.h"
#include "dji_motor.h"
#include "communicate.h"


const uint8_t vofa_justfloat_tail[4]={0x00,0x00,0x80,0x7f};
const uint8_t vofa_firewater_tail = '\n';

VofaTypeDef Vofa = {0};




void Vofa_Init(void)
{
	Vofa.flag = 1;
	Vofa.tick = 0;
	
	// 初始化数据尾帧指针
	Vofa.just_float.tail = vofa_justfloat_tail;
	Vofa.fire_water.tail = &vofa_firewater_tail;
	
	// 清空缓冲区
	memset(Vofa.just_float.tx_buff, 0, VOFA_BUFFER_LENGTH);
	memset(Vofa.just_float.rx_buff, 0, VOFA_BUFFER_LENGTH);
	memset(Vofa.fire_water.tx_buff, 0, VOFA_BUFFER_LENGTH);
	memset(Vofa.fire_water.rx_buff, 0, VOFA_BUFFER_LENGTH);

	HAL_UARTEx_ReceiveToIdle_DMA(&VOFA_USE_USART, Vofa.just_float.rx_buff, VOFA_BUFFER_LENGTH);		// 串口空闲中断接收DMA
	__HAL_DMA_DISABLE_IT(&VOFA_USE_USART_RX_DMA, DMA_IT_HT);		// 关闭DMA接收一半中断
	__HAL_UART_ENABLE_IT(&VOFA_USE_USART, UART_IT_IDLE);			// 使能串口空闲中断
}


/**
 * @brief 发送JustFloat协议数据
 * @param data_array: 浮点数数组
 * @param data_count: 数据个数（不能超过JUSTFLOAT_CH_COUNT）
 */
void Vofa_JustFloat_Send(float *data_array, uint8_t data_count)
{
    if(data_array == NULL || data_count == 0 || data_count > JUSTFLOAT_CH_COUNT) {
        return;
    }
    
    // 计算数据总大小
    uint16_t data_size = data_count * sizeof(float);
    uint16_t total_size = data_size + 4; // 数据 + 尾帧
    
    if(total_size > VOFA_BUFFER_LENGTH) {
        return;
    }
    
    // 复制数据到缓冲区
    memcpy(Vofa.just_float.tx_buff, data_array, data_size);
    
    // 添加协议尾帧8
    memcpy(Vofa.just_float.tx_buff + data_size, 
           Vofa.just_float.tail, 4);
    
    // 发送数据（需要根据实际硬件修改串口）
    HAL_UART_Transmit_DMA(&VOFA_USE_USART, Vofa.just_float.tx_buff, total_size);
}


void Vofa_JustFloat_Receive(void)
{
	HAL_UART_DMAStop(&VOFA_USE_USART);

	// sscanf((char*)Vofa.just_float.rx_buff, "kp:%f\n", &Gimbal.yaw_motor.pid_angle.pid_data.kp);
	// sscanf((char*)Vofa.just_float.rx_buff, "ki:%f\n", &Gimbal.yaw_motor.pid_angle.pid_data.ki);
	// sscanf((char*)Vofa.just_float.rx_buff, "kd:%f\n", &Gimbal.yaw_motor.pid_angle.pid_data.kd);
	// sscanf((char*)Vofa.just_float.rx_buff, "imax:%f\n", &Gimbal.yaw_motor.pid_angle.pid_data.ki_max);
	// sscanf((char*)Vofa.just_float.rx_buff, "outmax:%f\n", &Gimbal.yaw_motor.pid_angle.pid_data.out_max);

	HAL_UARTEx_ReceiveToIdle_DMA(&VOFA_USE_USART, Vofa.just_float.rx_buff, VOFA_BUFFER_LENGTH);		// 重新开启串口空闲中断接收DMA
	__HAL_DMA_DISABLE_IT(&VOFA_USE_USART_RX_DMA, DMA_IT_HT);		// 关闭DMA接收一半中断
	memset(Vofa.just_float.rx_buff, 0, VOFA_BUFFER_LENGTH);
}


/**
 * @brief 发送FireWater协议数据
 * @param data : 单个浮点数数据
 * @param data_num: 数据个数（不能超过FIREWATER_CH_COUNT）
 */
void Vofa_FireWater_Send(float data, uint32_t data_num)
{
	if(data == NULL || data_num > FIREWATER_CH_COUNT){
		return;
	}
	
	sprintf((char*)Vofa.fire_water.tx_buff, "%.1f%s", data, Vofa.fire_water.tail);
	
	HAL_UART_Transmit(&VOFA_USE_USART, Vofa.fire_water.tx_buff, sizeof(Vofa.fire_water.tx_buff), 1000);
}


void Vofa_Task(void)
{
	if(!Vofa.flag || Vofa.tick < VOFA_TICK_TIME){ 
		return;
	}
		
	Vofa.tick = 0;
	
	// float gimbal_yaw_angle[] = {Gimbal.yaw_motor.target_angle, Gimbal.yaw_motor.current_angle, Hi14.hi_data.euler.yaw, At9s.at9s_rc.left_x};
	float motor_angle[] ={Chassis.chassis_yaw_motor[DJI_MOTOR_6020_CHASSIS_YAW_RX_1].target_angle, 
						  Chassis.chassis_yaw_motor[DJI_MOTOR_6020_CHASSIS_YAW_RX_1].current_angle,
						  Can_Communicate.data.yaw};
	// float fiction_speed[] = {Shoot.friction_motor[0].target_speed, Shoot.friction_motor[0].current_speed};

	// 临时调试：直接查看原始编码器值和映射结果
	Vofa_JustFloat_Send(motor_angle, 3);
}


void Vofa_Timing_Callback(void)
{
	if(Vofa.tick < VOFA_TICK_TIME){
		Vofa.tick++;
	}
}


void Vofa_Rx_Callback(uint16_t size)
{
	Vofa_JustFloat_Receive();
}
