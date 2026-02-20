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

// 写入数据 (也可以直接用 app_recv_data_fifo_enter 通用版)
uint16_t fifo_write(fifo_t *fifo, const uint8_t *p_data, uint16_t len)
{
    uint16_t i;
    for(i = 0; i < len; i++)
    {
        // 写入数据到 head 位置
        fifo->p_buf[fifo->head] = *p_data++;
        
        // head 指针前进循环
        fifo->head++;
        if (fifo->head >= fifo->size)
        {
            fifo->head = 0;
        }

        // 如果逻辑上满了，同步 tail (覆盖旧数据)
        if (fifo->head == fifo->tail)
        {
            // tail 也前进一格，避免出现空数据
            fifo->tail++;
            if (fifo->tail >= fifo->size)
            {
                fifo->tail = 0;
            }
            fifo->is_full = true; // 满标记/覆盖
        }
    }
    return len;
}

// 读取数据 (注意后面我们也要用得到的)
uint16_t fifo_read(fifo_t *fifo, uint8_t *p_data, uint16_t len)
{
    uint16_t i;
    // 简单粗暴：如果没有数据可读就写一个逻辑
    for(i = 0; i < len; i++)
    {
        if(fifo->head == fifo->tail) // 空
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
    return i; // 返回实际读取得字节数
}
