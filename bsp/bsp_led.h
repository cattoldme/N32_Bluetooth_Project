#ifndef __bsp_LED_H__
#define __bsp_LED_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32wb03x.h"

#define LED1_PORT   GPIOA
#define LED1_PIN    GPIO_PIN_2
#define LED2_PORT   GPIOA
#define LED2_PIN    GPIO_PIN_3
#define LED3_PORT   GPIOB
#define LED3_PIN    GPIO_PIN_0

void LedInit(GPIO_Module* GPIOx, uint16_t Pin);
void LedOn(GPIO_Module* GPIOx, uint16_t Pin);
void LedOff(GPIO_Module* GPIOx, uint16_t Pin);
void LedBlink(GPIO_Module* GPIOx, uint16_t Pin);

//project led method start
void Led_Init(void);
//project led method end
#ifdef __cplusplus
}
#endif

#endif
