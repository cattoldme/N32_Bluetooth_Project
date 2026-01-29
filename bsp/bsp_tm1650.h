#ifndef __bsp_TM1650_H__
#define __bsp_TM1650_H__

#include "n32wb03x.h"

#include "bsp_usart.h"
#include "bsp_led.h"
#include "ns_log.h"
#include "bsp_tm1650.h"
#include "ns_delay.h"
#include "bsp_frame.h"
#include "bsp_relay.h"
#include "bsp_buzzer.h"

#define TM1650_GPIO_PORT    GPIOB
#define TM1650_GPIO_CLK     RCC_APB2_PERIPH_GPIOB
#define TM1650_PIN_SCL      GPIO_PIN_7
#define TM1650_PIN_SDA      GPIO_PIN_6

//cmd
#define TM1650_CMD_MODE     0x48  // CMD_MODE

//DISPLAY ADDR CMD
#define TM1650_ADDR_DIG1    0x68
#define TM1650_ADDR_DIG2    0x6A
#define TM1650_ADDR_DIG3    0x6C
#define TM1650_ADDR_DIG4    0x6E

//Bright 0-7
#define TM1650_BRIGHT_1     0x10  // 1 level bright
#define TM1650_BRIGHT_2     0x20
#define TM1650_BRIGHT_3     0x30
#define TM1650_BRIGHT_4     0x40
#define TM1650_BRIGHT_5     0x50
#define TM1650_BRIGHT_6     0x60
#define TM1650_BRIGHT_7     0x70
#define TM1650_BRIGHT_8     0x00  // 8 level bright

// 0-9, A-F, BLANK
extern const uint8_t TM1650_TubeFont[]; 

void TM1650_Init(void);
void TM1650_SetDisplay(uint8_t brightness, uint8_t state);
void TM1650_WriteData(uint8_t addr, uint8_t data);
void TM1650_DisplayInt(uint16_t num);
void TM1650_DisplayTime(uint16_t total_seconds);

#endif
