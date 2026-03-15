#include "nx_topic.h"



NxTopicTyepDef nx_topic;

uint8_t rx_buffer[NX_RX_BUFFER_LENGTH];


void Nx_Topic_Init(void)
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, NX_RX_BUFFER_LENGTH);
    __HAL_DMA_DISABLE_IT(&hdma_usart1_rx,DMA_IT_HT);
}


/**
 * @brief 解析一帧或多帧数据（协议格式：HEAD(0x51) + LEN + CMD + PAYLOAD + CHECKSUM）
 * @param data 数据缓冲区指针
 * @param len  数据长度
 */
void Nx_Parse_Data(NxTopicTyepDef *nx_data, uint8_t *data, uint16_t len)
{
    uint16_t idx = 0;

    while (idx + 3 < len)   // 至少需要 HEAD + LEN + 1字节CMD才能解析长度
    {
        /* 查找帧头（注意宏定义为 NX_RX_TAIL，但实际是帧头） */
        if (data[idx] != NX_RX_TAIL)
        {
            idx++;
            continue;
        }

        // uint8_t head   = data[idx];
        uint8_t length = data[idx + 1];   // LEN = CMD + PAYLOAD 长度
        uint8_t cmd    = data[idx + 2];

        /* 计算整帧长度：HEAD(1) + LEN(1) + length + CHECKSUM(1) */
        uint16_t frame_len = 1 + 1 + length + 1;   // 即 length + 3

        /* 检查剩余数据是否足够一帧 */
        if (idx + frame_len > len)
        {
            break;   // 数据不足，等待下一批
        }

        /* 校验和计算：所有字节（除校验和自身）累加取低8位 */
        uint8_t checksum = 0;
        for (uint16_t i = idx; i < idx + frame_len - 1; i++)
        {
            checksum += data[i];
        }
        if (checksum != data[idx + frame_len - 1])
        {
            /* 校验失败，跳过当前字节继续搜索 */
            idx++;
            continue;
        }

        /* 校验通过，根据命令字处理数据 */
        uint8_t *payload = &data[idx + 3];   // 指向数据载荷

        if (cmd == NX_TYPE_HEART && length == 5)   // 心跳帧：seq(1)+timestamp(4)
        {
            nx_data->heartbeat.seq = payload[0];
            nx_data->heartbeat.timestamp = (uint32_t)payload[1] |
                                            ((uint32_t)payload[2] << 8) |
                                            ((uint32_t)payload[3] << 16) |
                                            ((uint32_t)payload[4] << 24);
        }
        else if (cmd == NX_TYPE_NAVIGATION && length == 17)   // 组合帧：长度应为17（CMD+PAYLOAD=1+17=18，length字段值为18？需核对）
        {
            /* 注意：协议文档中组合帧 LEN = 0x12 (18)，即CMD(1)+PAYLOAD(17)=18 */
            /* 但此处我们使用用户定义的 NX_TYPE_NAVIGATION = 0x04，长度检查按实际payload长度 */
            /* 如果length值为18，则payload长度为17，符合预期 */

            // p->target_found = payload[0];
            // p->target_id    = payload[1];
            // p->yaw          = (int16_t)(payload[2] | ((int16_t)payload[3] << 8));
            // p->pitch        = (int16_t)(payload[4] | ((int16_t)payload[5] << 8));
            // p->distance     = (uint16_t)(payload[6] | ((uint16_t)payload[7] << 8));
            // p->nav_mode     = payload[8];
            // p->target_x     = (int16_t)(payload[9] | ((int16_t)payload[10] << 8));
            // p->target_y     = (int16_t)(payload[11] | ((int16_t)payload[12] << 8));
            // p->target_yaw   = (uint16_t)(payload[13] | ((uint16_t)payload[14] << 8));
            // p->velocity     = (uint16_t)(payload[15] | ((uint16_t)payload[16] << 8));

            nx_data->combined.target_found = payload[0];
            nx_data->combined.target_id    = payload[1];
            nx_data->combined.yaw          = (int16_t)(payload[2] | ((int16_t)payload[3] << 8));
            nx_data->combined.pitch        = (int16_t)(payload[4] | ((int16_t)payload[5] << 8));
            nx_data->combined.distance     = (uint16_t)(payload[6] | ((uint16_t)payload[7] << 8));
            nx_data->combined.nav_mode     = payload[8];
            nx_data->combined.target_x     = (int16_t)(payload[9] | ((int16_t)payload[10] << 8));
            nx_data->combined.target_y     = (int16_t)(payload[11] | ((int16_t)payload[12] << 8));
            nx_data->combined.target_yaw   = (uint16_t)(payload[13] | ((uint16_t)payload[14] << 8));
            nx_data->combined.velocity     = (uint16_t)(payload[15] | ((uint16_t)payload[16] << 8));
        }
        /* 可根据需要扩展其他命令字，如 NX_TYPE_VISION (0x02) 等 */

        /* 处理完一帧，移动索引到下一帧起始 */
        idx += frame_len;
    }
}


void Nx_Rx_Handler(uint16_t len)
{	
    /* 解析接收到的数据 */
    // Nx_Parse_Data(&nx_topic, nx_topic.rx_buffer, len);

    /* 处理完毕后，重新开启DMA接收 */
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, NX_RX_BUFFER_LENGTH);
    __HAL_DMA_DISABLE_IT(&hdma_usart1_rx,DMA_IT_HT);
}


