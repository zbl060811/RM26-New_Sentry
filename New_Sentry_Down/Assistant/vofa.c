#include "vofa.h"
#include <stdio.h>
#include <string.h>
#include "chassis.h"
#include "communicate.h"


const uint8_t vofa_justfloat_tail[4]={0x00,0x00,0x80,0x7f};
const uint8_t vofa_firewater_tail = '\n';

VofaTypeDef Vofa_Data = {0};




void Vofa_Init(void)
{
	Vofa_Data.flag = 1;
	Vofa_Data.tick = 0;
	
	// ��ʼ������βָ֡��
	Vofa_Data.tx_float.tail = vofa_justfloat_tail;
	Vofa_Data.tx_water.tail = &vofa_firewater_tail;
	
	// ��ջ�����
	memset(Vofa_Data.tx_float.fbuff, 0, VOFA_BUFFER_LENGTH);
	memset(Vofa_Data.tx_water.wbuff, 0, VOFA_BUFFER_LENGTH);
}


/**
 * @brief ����JustFloatЭ������
 * @param data_array: ����������
 * @param data_count: ���ݸ��������ܳ���JUSTFLOAT_CH_COUNT��
 */
void Vofa_JustFloat_Send(float *data_array, uint8_t data_count)
{
    if(data_array == NULL || data_count == 0 || data_count > JUSTFLOAT_CH_COUNT) {
        return;
    }
    
    // ���������ܴ�С
    uint16_t data_size = data_count * sizeof(float);
    uint16_t total_size = data_size + 4; // ���� + β֡
    
    if(total_size > VOFA_BUFFER_LENGTH) {
        return;
    }
    
    // �������ݵ�������
    memcpy(Vofa_Data.tx_float.fbuff, data_array, data_size);
    
    // ����Э��β֡
    memcpy(Vofa_Data.tx_float.fbuff + data_size, 
           Vofa_Data.tx_float.tail, 4);
    
    // �������ݣ���Ҫ����ʵ��Ӳ���޸Ĵ��ڣ�
    HAL_UART_Transmit_DMA(&VOFA_USE_USART, Vofa_Data.tx_float.fbuff, total_size);
}


void Vofa_FireWater_Send(float data, uint32_t data_num)
{
	if(data == NULL || data_num > FIREWATER_CH_COUNT){
		return;
	}
	
	sprintf((char*)Vofa_Data.tx_water.wbuff, "%.1f%s", data, Vofa_Data.tx_water.tail);
	
	HAL_UART_Transmit(&VOFA_USE_USART, Vofa_Data.tx_water.wbuff, sizeof(Vofa_Data.tx_water.wbuff), 1000);
}


void Vofa_Task(void)
{
	if(!Vofa_Data.flag){ 
		return;
	}
	
	if(Vofa_Data.tick < VOFA_TICK_TIME){
		return;
	}
		
	Vofa_Data.tick = 0;
	
	float motor_speed[] = {Chassis.chassis_yaw_motor[0].target_angle, Chassis.chassis_yaw_motor[0].current_angle, Can_Communicate.data.yaw, Can_Communicate.data.yaw_rate, Can_Communicate.data.pitch, Can_Communicate.data.rc_left_x};
	Vofa_JustFloat_Send(motor_speed, 6);
}


void Vofa_Timing_Callback(void)
{
	if(Vofa_Data.tick < VOFA_TICK_TIME){
		Vofa_Data.tick++;
	}
}


