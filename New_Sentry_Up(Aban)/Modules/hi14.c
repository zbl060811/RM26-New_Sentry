#include "hi14.h"


HiTypeDef Hi14;



/**
  * @brief  超核模块初始化
  * @param  none
  * @return none
  */
void Hi14_Init(void)
{
    Hi14.flag = 1;
    Hi14.tick = 0;
    
    HAL_Delay(300);			// 延时必须200ms以上
    // 启用带IDLE中断的DMA接收	
    HAL_UARTEx_ReceiveToIdle_DMA(&HI14_USE_USART, Hi14.rx_buffer, RX_BUFFER_SIZE);
	// 禁用半传输中断，减少中断次数
    __HAL_DMA_DISABLE_IT(&HI14_USE_USART_DMA_RX, DMA_IT_HT); 
}


/**
  * @brief  发送超核命令
  * @param  cmd: 命令字符串
  * @return none
  */
void Hi14_Send_Cmd(char *cmd)
{
	HAL_UART_Transmit_DMA(&HI14_USE_USART, (uint8_t*)cmd, strlen(cmd));
}


/**
  * @brief  复位超核模块
  * @param  none
  * @return none
  */
void Hi14_Set_Reset(void)
{
	char cmd[10] = {0};
	sprintf(cmd, "REBOOT\r\n");
	Hi14_Send_Cmd(cmd);
}


/**
  * @brief  保存超核模块配置
  * @param  none
  * @return none
  */
void Hi14_Set_Save(void)
{
	char cmd[20] = {0};
	sprintf(cmd, "SAVECONFIG\r\n");
	Hi14_Send_Cmd(cmd);
}


/**
  * @brief  设置超核模块波特率
  * @param  baudrate: 波特率
  * @return none
  */
void Hi14_Set_Baud(uint32_t baudrate)
{
	char cmd[30] = {0};
	sprintf(cmd, "SERIALCONFIG %d\r\n", baudrate);
	Hi14_Send_Cmd(cmd);
}


/**
  * @brief  CRC16计算
  * @param  currentCrc: 当前CRC值
  * @param  src: 数据源
  * @param  lengthInBytes: 数据长度
  * @return none
  */
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


/**
  * @brief  解析超核模块数据
  * @param  data: 数据源
  * @param  size: 数据长度
  * @return 解析的帧数
  */
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
			Hi14_Parse_IMUFrame((HiDataTypeDef*)(frame_start+6));		// 强制转换为HiDataTypeDef类型解算
			frame_parsed++;
		}
		
		// 6. 移动到下一帧（跳过已处理数据）
        i += total_frame_len;
	}
	return frame_parsed; // 返回解析的帧数
}


/**
  * @brief  解析超核模块数据帧
  * @param  frame: 数据帧
  * @return none
  */
void Hi14_Parse_IMUFrame(const HiDataTypeDef* frame) 
{
	// 状态字检查
	Hi14_Check_Status(frame->main_status, &Hi14.hi_status);
	
	// 温度、气压获取
	Hi14.hi_data.temperature = frame->temperature;
	Hi14.hi_data.air_pressure = frame->air_pressure;
	
	// 加速度数据映射,直接使用结构体指针访问，避免内存拷贝
	Hi14.hi_data.acc.ax = frame->acc.ax;
	Hi14.hi_data.acc.ay = frame->acc.ay;
	Hi14.hi_data.acc.az = frame->acc.az;
	
	// 角速度数据映射
    Hi14.hi_data.gyro.gx = frame->gyro.gx;	// X轴 -> roll_rate
	Hi14.hi_data.gyro.gy = frame->gyro.gy;	// Y轴 -> pitch_rate
	Hi14.hi_data.gyro.gz = frame->gyro.gz;	// Z轴 -> yaw_rate 
	
	// 磁强度数据映射
	Hi14.hi_data.magent.mx = frame->magent.mx;
	Hi14.hi_data.magent.my = frame->magent.my;
	Hi14.hi_data.magent.mz = frame->magent.mz;
	
    // 欧拉角数据映射
    Hi14.hi_data.euler.roll = frame->euler.roll;
	Hi14.hi_data.euler.pitch= frame->euler.pitch;
	Hi14.hi_data.euler.yaw = frame->euler.yaw;
    
	// 四元数数据映射
	Hi14.hi_data.quater.qw = frame->quater.qw;
	Hi14.hi_data.quater.qx = frame->quater.qx;
	Hi14.hi_data.quater.qy = frame->quater.qy;
	Hi14.hi_data.quater.qz = frame->quater.qz;
}


/**
  * @brief  超核模块状态检查
  * @param  main_status: 主状态字
  * @param  status: 状态结构体
  * @return none
  */
void Hi14_Check_Status(uint16_t main_status, HiIMUStatusTypeDef* status)
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


/**
  * @brief  超核模块接收数据回调
  * @param  size: 接收到的数据长度
  * @return none
  */
void Hi14_Rx_Callback(uint16_t size)
{
	Hi_Parse_Data(Hi14.rx_buffer, size);
	Hi14.tick = 0;
}


/**
  * @brief  超核模块定时回调
  * @param  none
  * @return none
  */
void Hi14_Timing_Callback(void)
{
	if(Hi14.tick < HI14_TICK_TIME){
		Hi14.tick++;
	}
}



