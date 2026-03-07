#include "hi12.h"



HiTypeDef Hi12 = {0,0,{0},{0},{0}};


void Hi12_Init(void)
{
	Hi12.flag = 1;
	Hi12.tick = 0;

	// 启用带IDLE中断的DMA接收
    HAL_UARTEx_ReceiveToIdle_DMA(&HI12_USE_USART, Hi12.rx_buffer, RX_BUFFER_SIZE);
	// 禁用半传输中断，减少中断次数
    __HAL_DMA_DISABLE_IT(&HI12_USE_USART_DMA_RX, DMA_IT_HT);
}


void Crc16_Update(uint16_t *currentCrc, const uint8_t *src, uint32_t lengthInBytes) 
{
    uint32_t crc = *currentCrc;
    for (uint32_t j = 0; j < lengthInBytes; ++j) {
        crc ^= (uint16_t)(src[j] << 8);
        for (uint32_t i = 0; i < 8; ++i) {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    *currentCrc = crc & 0xFFFF;
}


uint16_t Hi_Parse_Data(uint8_t *data, uint16_t size)
{
	uint16_t frame_parsed = 0;
	uint16_t i = 0;
	
	// 预计算最小处理长度
	const uint16_t min_frame_size = 6 + sizeof(HiDataTypeDef);
	
	while(i <= size - min_frame_size){
		// 1. 快速帧头搜索（4字节对齐优化）
        uint32_t *word_ptr = (uint32_t*)(data + i);
        uint32_t header_pattern = SOF_FRAME_1 | (SOF_FRAME_2 << 8);
		
		// 检查当前32位字是否包含帧头
        if ((*word_ptr & 0xFFFF) != header_pattern) {
            i++;
            continue;
        }

		// 2. 提取帧信息
        const uint8_t* frame_start = data + i;
        uint16_t payload_len = frame_start[2] | (frame_start[3] << 8);
        uint16_t frame_crc = frame_start[4] | (frame_start[5] << 8);
		
		// 3. 快速完整性检查
        uint16_t total_frame_len = 6 + payload_len;
        if (i + total_frame_len > size) {
            break; // 缓冲区数据不足
        }

		 // 4. 快速CRC校验
		uint16_t crc = 0;
		Crc16_Update(&crc, frame_start, 4);
		Crc16_Update(&crc, frame_start + 6, payload_len);
		
        if (crc != frame_crc) {
            i += 2; // 跳过错误帧头
            continue;
        }
		
		// 5. 解析数据包
		if(frame_start[6] == DATA_PACKET_TAG && payload_len >= sizeof(HiDataTypeDef))
		{
			Hi12_Parse_IMUFrame((HiDataTypeDef*)(frame_start+6));		// 强制转换为HiDataTypeDef类型解算
			frame_parsed++;
		}
		
		// 6. 移动到下一帧（跳过已处理数据）
        i += total_frame_len;
	}
	return frame_parsed; // 返回解析的帧数
}


void Hi12_Parse_IMUFrame(const HiDataTypeDef* frame) 
{
	// 状态字检查
	Hi12_Check_Status(frame->main_status, &Hi12.hi_status);
	
	// 温度、气压获取
	Hi12.hi_data.temperature = frame->temperature;
	Hi12.hi_data.air_pressure = frame->air_pressure;
	
	// 加速度数据映射,直接使用结构体指针访问，避免内存拷贝
	Hi12.hi_data.acc.ax = frame->acc.ax;
	Hi12.hi_data.acc.ay = frame->acc.ay;
	Hi12.hi_data.acc.az = frame->acc.az;
	
	// 角速度数据映射
    Hi12.hi_data.gyro.gx = frame->gyro.gx;	// X轴 -> roll_rate
	Hi12.hi_data.gyro.gy = frame->gyro.gy;	// Y轴 -> pitch_rate
	Hi12.hi_data.gyro.gz = frame->gyro.gz;	// Z轴 -> yaw_rate 
	
	// 磁强度数据映射
	Hi12.hi_data.magent.mx = frame->magent.mx;
	Hi12.hi_data.magent.my = frame->magent.my;
	Hi12.hi_data.magent.mz = frame->magent.mz;
	
    // 欧拉角数据映射
    Hi12.hi_data.euler.roll = frame->euler.roll;
	Hi12.hi_data.euler.pitch= frame->euler.pitch;
	Hi12.hi_data.euler.yaw = frame->euler.yaw;
    
	// 四元数数据映射
	Hi12.hi_data.quater.qw = frame->quater.qw;
	Hi12.hi_data.quater.qx = frame->quater.qx;
	Hi12.hi_data.quater.qy = frame->quater.qy;
	Hi12.hi_data.quater.qz = frame->quater.qz;
}


void Hi12_Check_Status(uint16_t main_status, HiIMUStatusTypeDef* status)
{
	status->raw_status = main_status;
	
	status->parsed.bias_convergence = (main_status & STATUS_WB_CONV) ? BIAS_STATUS_POOR : BIAS_STATUS_GOOD;		// 获取零偏收敛
	status->parsed.mag_environment = (main_status & STATUS_MAG_DIST) ? MAG_ENV_DISTURBED : MAG_ENV_GOOD;		// 获取磁场干扰状态
	status->parsed.mag_mode = (main_status & STATUS_MAG_AIDING) ? MAG_MODE_9AXIS : MAG_MODE_6AXIS;		// 磁参与姿态解算
	status->parsed.time_sync = (main_status & STATUS_UTC_TIME) ? TIME_UTC_SYNCED : TIME_LOCAL;		// 时间同步
	status->parsed.sout_pulse = (main_status & STATUS_SOUT_PULSE) ? SOUT_PULSE_OUTPUT : SOUT_NO_PULSE;	// SOUT脉冲输出
	
	// 计算质量标志
    status->quality.is_healthy = (status->parsed.bias_convergence == BIAS_STATUS_GOOD) &&
                                (status->parsed.mag_environment == MAG_ENV_GOOD);
	
    status->quality.needs_calibration = (status->parsed.bias_convergence == BIAS_STATUS_POOR);
    status->quality.has_mag_interference = (status->parsed.mag_environment == MAG_ENV_DISTURBED);
    status->quality.time_synced = (status->parsed.time_sync == TIME_UTC_SYNCED);
}


void Hi12_Rx_Callback(uint16_t size)
{
	Hi_Parse_Data(Hi12.rx_buffer, size);
	Hi12.tick = 0;
}


void Hi_Timing_Callback(void)
{
	if(Hi12.tick < HI12_TICK_TIME){
		Hi12.tick++;
	}
}
