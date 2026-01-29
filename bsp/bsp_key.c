#include "bsp_key.h"

volatile uint32_t g_sys_tick = 0;

void KeyInit(void)
{
    //RCC
    //RCC_ConfigPclk1(RCC_HCLK_DIV4);
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM6, ENABLE);

    //NVIC
    NVIC_InitType NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel          = TIM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority  = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd       = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //2keys_GPIO
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin          = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Pull    = GPIO_PULL_UP;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_LOW;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
    //TIM
    uint16_t PrescalerValue = 63;
    TIM_TimeBaseInitType TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.Period    = 999;
    TIM_TimeBaseStructure.Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.ClkDiv    = 0;
    TIM_TimeBaseStructure.CntMode   = TIM_CNT_MODE_UP;

    TIM_InitTimeBase(TIM6, &TIM_TimeBaseStructure);
    TIM_ConfigPrescaler(TIM6, PrescalerValue, TIM_PSC_RELOAD_MODE_IMMEDIATE);
    TIM_ConfigInt(TIM6, TIM_INT_UPDATE, ENABLE);
    TIM_Enable(TIM6, ENABLE);

}

void Delay_ms(uint32_t ms)
{
    uint32_t start_tick = g_sys_tick;
    while((g_sys_tick - start_tick) < ms);
}
