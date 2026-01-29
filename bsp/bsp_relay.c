#include "bsp_relay.h"

void RelayInit(GPIO_Module* GPIOx, uint16_t Pin)
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

	GPIO_ResetBits(GPIOx, Pin);//init off
	
	if (Pin <= GPIO_PIN_ALL)
	{
		GPIO_InitStruct(&GPIO_InitStructure);
		GPIO_InitStructure.Pin = Pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);
	}
	
}

void RelayOn(GPIO_Module* GPIOx, uint16_t Pin)
{    
    GPIO_SetBits(GPIOx, Pin);
}

void RelayOff(GPIO_Module* GPIOx, uint16_t Pin)
{
    GPIO_ResetBits(GPIOx, Pin);
}
