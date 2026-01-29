#include "bsp_timer.h"

volatile uint8_t timer_state = 0;

volatile uint32_t time_sec = 0;
volatile uint32_t Tick_num = 0;

void SysTick_Handler(void)
{
    Tick_num++;
}


int timer_init(void)
{
   TM1650_Init();
   TM1650_SetDisplay(TM1650_BRIGHT_3,1);
	 TM1650_DisplayInt(0); 	//数码管默认显示00:00

   SystemCoreClockUpdate();
   SysTick_Config(SystemCoreClock / BSP_SYSTICK_TICK);  //默认64MHz

   return true;
}

void timer_run(void)
{
    // 静态变量
    static uint32_t old_sec_time = 0;
    static uint32_t old_do_time = 0;
    static uint8_t  last_state = 0; 
    
    // 待机心跳计时
    static uint32_t led1_idle_time = 0; 

    // ===========================================
    // 逻辑 A: 工作模式 (Running)
    // ===========================================
    if (timer_state == 1)
    {
        // 【关键修改】检测“刚刚进入”状态1
        // 无论是从 0(停止) 还是 2(暂停) 变成 1，都要重置基准时间
        if(last_state != 1) 
        {
            old_do_time = Tick_num;
            old_sec_time = Tick_num;
            
            // 确保进入运行时 LED1 熄灭
            LedOff(LED1_PORT, LED1_PIN);
            // 确保继电器是吸合的 (如果是从暂停回来，它本身就是吸合的，多设一次无妨)
            // RelayOn(RELAY_PORT, RELAY_PIN); 
        }

        // 1. LED3 快闪 + 屏幕刷新
        if((Tick_num - old_do_time) >= 50000)
        {
            LedBlink(LED3_PORT,LED3_PIN);
            TM1650_DisplayTime(time_sec);
            old_do_time = Tick_num;
        }

        // 2. 倒计时逻辑
        if((time_sec > 0) && (Tick_num - old_sec_time) >= 100000)
        {
            time_sec--;
            old_sec_time = Tick_num;
        }
        else if(time_sec == 0)
        {
            timer_state = 0; // 时间到，转入停止
        }
    }
    // ===========================================
    // 【新增】逻辑 C: 暂停模式 (Pause)
    // LED3 常亮 (不闪)，时间静止
    // ===========================================
    else if (timer_state == 2)
    {
        // 检测刚刚进入暂停
        if (last_state != 2)
        {
            // 将 LED3 设为常亮，表示“冻结”状态
            // (如果想让它灭，这里改成 LedOff 即可)
            // GPIO_ResetBits 通常是点亮，根据你的宏定义修改
            LedOn(LED3_PORT, LED3_PIN); 
        }
        
        // 即使暂停，也要刷新数码管，否则数码管可能会灭或者闪烁
        // 这里不需要高频刷新，只要保证显示当前暂定的时间即可
        if((Tick_num - old_do_time) >= 50000)
        {
            TM1650_DisplayTime(time_sec); // 显示静止的时间
            old_do_time = Tick_num;
        }
    }
    // ===========================================
    // 逻辑 B: 待机模式 (Stop)
    // ===========================================
    else if (timer_state == 0)
    {
        // 检测下降沿 (从1或2 变成 0)
        if (last_state != 0)
        {
            LedOff(LED3_PORT,LED3_PIN);      
            RelayOff(RELAY_PORT, RELAY_PIN); 
            TM1650_DisplayTime(0);
            
            led1_idle_time = Tick_num; // 重置待机心跳
        }
        
        // LED1 慢闪心跳
        if ((Tick_num - led1_idle_time) >= 100000) 
        {
            LedBlink(LED1_PORT, LED1_PIN);
            led1_idle_time = Tick_num;
        }
    }
    
    // 更新历史状态
    last_state = timer_state;
}
