#ifndef __bsp_relay_H__
#define __bsp_relay_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32wb03x.h"

#define RELAY_PORT   GPIOB
#define RELAY_PIN    GPIO_PIN_11

void RelayInit(GPIO_Module* GPIOx, uint16_t Pin);
void RelayOn(GPIO_Module* GPIOx, uint16_t Pin);
void RelayOff(GPIO_Module* GPIOx, uint16_t Pin);

#ifdef __cplusplus
}
#endif

#endif
