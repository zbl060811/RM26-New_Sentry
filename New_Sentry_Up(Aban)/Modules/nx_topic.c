#include "nx_topic.h"



VisionTopicTyepDef vision_topic;





void Nx_Topic_Init(VisionTopicTyepDef *topic)
{
	HAL_UARTEx_ReceiveToIdle_DMA(&NX_USE_USART, topic->rx_buffer, NX_RX_BUFFER_LENGTH);
    __HAL_DMA_DISABLE_IT(&NX_USE_USART_RX_DMA,DMA_IT_HT);
}


/**
 * @brief 计算校验和
 * @param buff 数据缓冲区
 * @param size 数据大小（包含HEAD到PAYLOAD）
 * @return 校验和
 */
static uint8_t Nx_CalculateChecksum(uint8_t *buff, uint8_t size)
{
    uint8_t sum = 0;
    for(uint8_t i = 0; i < size; i++)
    {
        sum += buff[i];
    }
    return sum;
}


/**
 * @brief 解析心跳帧 (CMD=0x03)
 * @param payload 数据载荷指针（指向CMD后面的数据）
 * @param parsed_frame 解析结果存储结构
 * 
 * 心跳帧格式：
 * HEAD(0x51) + LEN(0x06) + CMD(0x03) + seq(1) + timestamp(4) + CHECKSUM
 * 示例: 51 06 03 2A 78 56 34 12 [CHECKSUM]
 * seq = 0x2A, timestamp = 0x12345678 (小端)
 */
static void Nx_ParseHeartbeatFrame(uint8_t *payload, NxParsedFrameTypeDef *parsed_frame)
{
    NxHeartbeatFrameTypeDef *heartbeat = &parsed_frame->data.heartbeat;
    
    // 解析seq (uint8_t)
    heartbeat->seq = payload[0];
    
    // 解析timestamp (uint32_t, 小端)
    heartbeat->timestamp = (uint32_t)(payload[1] | 
                                      (payload[2] << 8) | 
                                      (payload[3] << 16) | 
                                      (payload[4] << 24));
    
    parsed_frame->valid = 1;
}


/**
 * @brief 解析组合帧数据 (CMD=0x04)
 * @param payload 数据载荷指针（指向CMD后面的数据）
 * @param parsed_frame 解析结果存储结构
 */
static void Nx_ParseCombinedFrame(uint8_t *payload, NxParsedFrameTypeDef *parsed_frame)
{
    NxCombinedFrameTypeDef *combined = &parsed_frame->data.combined;
    
    // 按小端格式解析数据
    combined->target_found = payload[0];
    combined->target_id = payload[1];
    
    // 解析int16_t (小端)
    combined->yaw = (int16_t)(payload[2] | (payload[3] << 8));
    combined->pitch = (int16_t)(payload[4] | (payload[5] << 8));
    
    // 解析uint16_t (小端)
    combined->distance = (uint16_t)(payload[6] | (payload[7] << 8));
    
    combined->nav_mode = payload[8];
    
    // 解析int16_t坐标 (小端)
    combined->target_x = (int16_t)(payload[9] | (payload[10] << 8));
    combined->target_y = (int16_t)(payload[11] | (payload[12] << 8));
    
    // 解析uint16_t (小端)
    combined->target_yaw = (uint16_t)(payload[13] | (payload[14] << 8));
    combined->velocity = (uint16_t)(payload[15] | (payload[16] << 8));
    
    parsed_frame->valid = 1;
}

/**
 * @brief 数据处理函数
 * @param topic 主题对象
 * @param buff 接收到的数据缓冲区（包含完整帧）
 * @param size 数据大小
 */
void Nx_Topic_Data_Process(VisionTopicTyepDef *topic, uint8_t *buff, uint8_t size)
{
    uint8_t checksum_calc;
    uint8_t checksum_recv;
    uint8_t payload_len;
    
    // 清理解析结果
    topic->parsed_frame.valid = 0;
    
    // 检查参数有效性
    if(buff == NULL || buff[0] != NX_RX_HEAD)
        return;
    
    // 检查帧长度（至少需要：HEAD + LEN + CMD + CHECKSUM）
    if(size < 4)
        return;
    
    // 获取长度字段（CMD+PAYLOAD的总长度）
    payload_len = buff[1];
    
    // 验证总帧长度（HEAD + LEN + CMD + PAYLOAD + CHECKSUM）
    // buff[1]是LEN，总帧长度 = 1(HEAD) + 1(LEN) + LEN + 1(CHECKSUM)·
    if(size != (1 + 1 + payload_len + 1))
        return;
    
    // 计算校验和（从HEAD到PAYLOAD）
    checksum_calc = Nx_CalculateChecksum(buff, 1 + 1 + payload_len);
    
    // 获取接收到的校验和（最后一个字节）
    checksum_recv = buff[1 + 1 + payload_len];
    
    // 校验和验证
    if(checksum_calc != checksum_recv)
        return;
    
    // 根据命令字解析数据
    topic->parsed_frame.cmd = buff[2];  // CMD在索引2
    
    switch(topic->parsed_frame.cmd)
    {
        case 0x03:  // 心跳帧
            // 心跳帧：CMD(1) + seq(1) + timestamp(4) = 6字节
            if(payload_len == 6)  // CMD + PAYLOAD的总长度
            {
                // payload指向seq数据（索引3开始）
                Nx_ParseHeartbeatFrame(&buff[3], &topic->parsed_frame);
            }
            break;
            
        case 0x04:  // 组合帧
            // 组合帧：CMD(1) + PAYLOAD(17) = 18字节
            if(payload_len == 18)
            {
                Nx_ParseCombinedFrame(&buff[3], &topic->parsed_frame);
            }
            break;
            
        default:
            // 未知命令字
            break;
    }
}


/**
 * @brief 获取最新的组合帧数据
 * @param topic 主题对象
 * @return 指向组合帧数据的指针，如果无效则返回NULL
 */
NxCombinedFrameTypeDef* Nx_GetCombinedFrame(VisionTopicTyepDef *topic)
{
    if(topic->parsed_frame.valid && topic->parsed_frame.cmd == 0x04)
    {
        return &topic->parsed_frame.data.combined;
    }
    return NULL;
}

/**
 * @brief 获取最新的心跳帧数据
 * @param topic 主题对象
 * @return 指向心跳帧数据的指针，如果无效则返回NULL
 */
NxHeartbeatFrameTypeDef* Nx_GetHeartbeatFrame(VisionTopicTyepDef *topic)
{
    if(topic->parsed_frame.valid && topic->parsed_frame.cmd == 0x03)
    {
        return &topic->parsed_frame.data.heartbeat;
    }
    return NULL;
}


/**
 * @brief 打印心跳帧数据（调试用）
 * @param heartbeat 心跳帧数据指针
 */
void Nx_PrintHeartbeatFrame(NxHeartbeatFrameTypeDef *heartbeat)
{
    if(heartbeat != NULL)
    {
        printf("Heartbeat - Seq: %d, Timestamp: %lu ms\r\n", 
               heartbeat->seq, heartbeat->timestamp);
    }
}


void Nx_Rx_Callback(VisionTopicTyepDef *topic, uint8_t *Buffer, uint8_t size)
{
    Nx_Topic_Data_Process(topic, Buffer, size);
    Nx_Topic_Init(topic);		// 重置Topic以准备接收下一帧数据
}

