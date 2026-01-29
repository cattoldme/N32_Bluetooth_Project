#include "bsp_led.h"

void LedInitSingle(GPIO_Module* GPIOx, uint16_t Pin)
{
	GPIO_InitType GPIO_InitStructure;
	
	assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
	
	if (GPIOx == GPIOA)
	{
			RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
	}
	else if (GPIOx == GPIOB)
	{
		RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
	}
	else
	{
		return;
	}

	GPIO_SetBits(GPIOx, Pin);//init off
	
	if (Pin <= GPIO_PIN_ALL)
	{
		GPIO_InitStruct(&GPIO_InitStructure);
		GPIO_InitStructure.Pin = Pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);
	}
	
}

void LedOn(GPIO_Module* GPIOx, uint16_t Pin)
{    
    GPIO_ResetBits(GPIOx, Pin);
}

void LedOff(GPIO_Module* GPIOx, uint16_t Pin)
{
    GPIO_SetBits(GPIOx, Pin);
}

void LedBlink(GPIO_Module* GPIOx, uint16_t Pin)
{
    GPIO_TogglePin(GPIOx, Pin);
}

void Led_Init(void)
{
	LedInitSingle(LED1_PORT, LED1_PIN);
	LedInitSingle(LED2_PORT, LED2_PIN);
	LedInitSingle(LED3_PORT, LED3_PIN);
}
