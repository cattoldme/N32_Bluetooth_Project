#ifndef __FIFO_H__
#define __FIFO_H__

#include <stdint.h>
#include <stdbool.h>

/* 定义一个结构体来管理环形缓冲区 */
typedef struct {
    uint8_t *p_buf;       // 指向实际内存数组的指针
    uint16_t size;        // 缓冲区总大小
    volatile uint16_t head; // 头指针 (写入口，相当于你代码里的 fifo_end)
    volatile uint16_t tail; // 尾指针 (读出口，相当于你代码里的 fifo_start)
    volatile bool is_full;  // 标记是否满（可选）
} fifo_t;

// 函数声明
void fifo_init(fifo_t *fifo, uint8_t *buffer, uint16_t size);
uint16_t fifo_write(fifo_t *fifo, const uint8_t *p_data, uint16_t len);
uint16_t fifo_read(fifo_t *fifo, uint8_t *p_data, uint16_t len);
uint16_t fifo_get_count(fifo_t *fifo); // 获取当前有多少数据

#endif