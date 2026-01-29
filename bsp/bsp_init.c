#include "bsp_init.h"

void bsp_init(void)
{
	//CRC ENABLE
	RCC_EnableAHBPeriphClk(RCC_AHB_PERIPH_CRC, ENABLE);
	
	TM1650_Init();
	Led_Init();
	BuzzerInit(BUZZER_PORT, BUZZER_PIN);
	RelayInit(RELAY_PORT, RELAY_PIN);
	timer_init();
	
}
