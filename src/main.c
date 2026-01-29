#include "BLE_Sys_Init.h"
#include "bsp_init.h"

int main(void)
{
	NS_LOG_INIT();
	app_ble_init();
	bsp_init();
	
	NS_LOG_INFO("INIT COMPLETE\r\n");

	while (1)
	{
		/*schedule all pending events*/
		rwip_schedule();
		ble_rx_msg();
		timer_run();
		//ns_sleep();
	}
}

/**
 * @brief  user handle before enter sleep mode
 * @param  
 * @return 
 * @note   
 */
void app_sleep_prepare_proc(void)
{
	
}

/**
 * @brief  user handle after wake up from sleep mode
 * @param  
 * @return 
 * @note   
 */
void app_sleep_resume_proc(void)
{
	
}
