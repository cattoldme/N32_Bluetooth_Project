#ifndef __bsp_timer_H
#define __bsp_timer_H

#include "n32wb03x.h"

#include "bsp_usart.h"
#include "bsp_led.h"
#include "ns_log.h"
#include "bsp_tm1650.h"
#include "ns_delay.h"
#include "bsp_frame.h"
#include "bsp_relay.h"
#include "bsp_buzzer.h"

#define SYSTICK_1MS ((uint32_t)1000)
#define SYSTICK_100MS ((uint32_t)100000)
#define SYSTICK_1S  ((uint32_t)1000000)

#define BSP_SYSTICK_TICK       SYSTICK_100MS

int timer_init(void);
void timer_run(void);

#endif

