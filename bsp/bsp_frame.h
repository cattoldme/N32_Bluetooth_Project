#ifndef __bsp_frame_h
#define __bsp_frame_h

#ifdef __cplusplus
extern "C" {
#endif

#include "n32wb03x.h"
#include "bsp_init.h"
#include "BLE_Sys_Init.h"

#define FRAME_HEAD  0xAA
#define FRAME_END   0x55

#define HEAD_SIZE 1
#define FUNC_CODE_SIZE 1
#define CRC_SIZE 2 //use crc-16/UMTS
#define LEN_SIZE 1
#define END_SIZE 1

#define RX_FIFO_SIZE 1000

typedef enum
{
	STATE_WAIT_HEAD,
	STATE_WAIT_FUNC,
	STATE_WAIT_LEN,
	STATE_WAIT_DATA,
	STATE_WAIT_CRC_1,
	STATE_WAIT_CRC_2,
	STATE_WAIT_END,
	STATE_SUCCESS,
}frame_t;

typedef enum
{
	FUNC_CODE_TIMING        = 0x0001,
	FUNC_CODE_TIMING_ACK,
	FUNC_CODE_STOP,
	FUNC_CODE_STOP_ACK,
	FUNC_CODE_GET_TIME,
	FUNC_CODE_GET_TIME_ACK,
	FUNC_CODE_PAUSE,
	FUNC_CODE_PAUSE_ACK,
	FUNC_CODE_RESUME,
	FUNC_CODE_RESUME_ACK,
	FUNC_CODE_MAX
}func_code_t;

typedef struct
{
	uint16_t func_code;
	uint16_t msg_len;
	uint8_t *data;
	uint16_t msg_crc;
}rx_msg_frame_t;

uint8_t app_recv_data_fifo_enter(const uint8_t *p_data, uint16_t len);
bool ble_rx_msg(void);

#ifdef __cplusplus
}
#endif
	
#endif
