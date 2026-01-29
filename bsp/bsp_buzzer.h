#ifndef __bsp_UART_H__
#define __bsp_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32wb03x.h"

#define BUZZER_PORT GPIOA
#define BUZZER_PIN  GPIO_PIN_6

void BuzzerInit(GPIO_Module* GPIOx, uint16_t Pin);
void BuzzerOn(GPIO_Module* GPIOx, uint16_t Pin);
void BuzzerOff(GPIO_Module* GPIOx, uint16_t Pin);

#ifdef __cplusplus
}
#endif

#endif
