#include "fifo.h"

// 初始化
void fifo_init(fifo_t *fifo, uint8_t *buffer, uint16_t size)
{
    fifo->p_buf = buffer;
    fifo->size = size;
    fifo->head = 0;
    fifo->tail = 0;
    fifo->is_full = false;
}

// 写入数据 (也就是你给出的 app_recv_data_fifo_enter 的通用版)
uint16_t fifo_write(fifo_t *fifo, const uint8_t *p_data, uint16_t len)
{
    uint16_t i;
    for(i = 0; i < len; i++)
    {
        // 写入数据到 head 位置
        fifo->p_buf[fifo->head] = *p_data++;
        
        // head 指针自增并回环
        fifo->head++;
        if (fifo->head >= fifo->size)
        {
            fifo->head = 0;
        }

        // 核心逻辑：如果追上了 tail (溢出覆盖)
        if (fifo->head == fifo->tail)
        {
            // tail 被迫向前推一步，丢弃最旧的数据
            fifo->tail++;
            if (fifo->tail >= fifo->size)
            {
                fifo->tail = 0;
            }
            fifo->is_full = true; // 标记满/溢出
        }
    }
    return len;
}

// 读出数据 (配套函数，你也需要用到的)
uint16_t fifo_read(fifo_t *fifo, uint8_t *p_data, uint16_t len)
{
    uint16_t i;
    // 这里应该先判断有没有数据可读，简单写一下逻辑
    for(i = 0; i < len; i++)
    {
        if(fifo->head == fifo->tail) // 空了
        {
            break;
        }
        
        *p_data++ = fifo->p_buf[fifo->tail];
        
        fifo->tail++;
        if (fifo->tail >= fifo->size)
        {
            fifo->tail = 0;
        }
    }
    return i; // 返回实际读到的字节数
}