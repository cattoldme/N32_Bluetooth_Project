#ifndef __FIFO_H__
#define __FIFO_H__

#include <stdint.h>
#include <stdbool.h>

/* 定义一个结构体来描述FIFO */
typedef struct {
    uint8_t *p_buf;       // 指向实际内存缓冲区指针
    uint16_t size;        // 缓冲区总大小
    volatile uint16_t head; // 头指针 (写位置，相当于之前的 fifo_end)
    volatile uint16_t tail; // 尾指针 (读位置，相当于之前的 fifo_start)
    volatile bool is_full;  // 标记是否已满(可选)
} fifo_t;

// 函数声明
void fifo_init(fifo_t *fifo, uint8_t *buffer, uint16_t size);
uint16_t fifo_write(fifo_t *fifo, const uint8_t *p_data, uint16_t len);
uint16_t fifo_read(fifo_t *fifo, uint8_t *p_data, uint16_t len);
uint16_t fifo_get_count(fifo_t *fifo); // 获取当前数据个数

#endif
