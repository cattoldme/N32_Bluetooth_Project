#ifndef __bsp_key_H__
#define __bsp_key_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32wb03x.h"

extern volatile uint32_t g_sys_tick;

void KeyInit(void);
void Delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif
