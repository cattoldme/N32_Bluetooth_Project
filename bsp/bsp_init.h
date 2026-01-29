#ifndef __bsp_init_H__
#define __bsp_init_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32wb03x.h"
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_tm1650.h"
#include "bsp_buzzer.h"
#include "bsp_frame.h"
#include "bsp_timer.h"

void bsp_init(void);

#ifdef __cplusplus
}
#endif

#endif
