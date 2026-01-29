#include "bsp_tm1650.h"

const uint8_t TM1650_TubeFont[] = 
{
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, // 0-9
    0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, // A-F
    0x00 // Blank
};

// SCL SDA HIGH OR LOW
#define SCL_H   GPIO_SetBits(TM1650_GPIO_PORT, TM1650_PIN_SCL)
#define SCL_L   GPIO_ResetBits(TM1650_GPIO_PORT, TM1650_PIN_SCL)
#define SDA_H   GPIO_SetBits(TM1650_GPIO_PORT, TM1650_PIN_SDA)
#define SDA_L   GPIO_ResetBits(TM1650_GPIO_PORT, TM1650_PIN_SDA)
#define SDA_READ (GPIO_ReadInputDataBit(TM1650_GPIO_PORT, TM1650_PIN_SDA))

//SDA CHANGE TO READ
//PIN6 , EVERY PIN HAS 2 BITS FOR MODE,SO 6X2
static void SDA_Mode_Out(void) 
{
    TM1650_GPIO_PORT->PMODE &= ~(0x00 << (6 * 2)); // INPUT WIPE
    TM1650_GPIO_PORT->PMODE |=  (0x01 << (6 * 2)); // OUTPUT
}

static void SDA_Mode_In(void) 
{
    TM1650_GPIO_PORT->PMODE &= ~(0x00 << (6 * 2)); // INPUT
    //PULL ?
}

//I2C START SIGNAL
static void TM1650_Start(void) 
{
    SDA_Mode_Out();
    SDA_H;
    SCL_H;
    delay_n_10us(1);
    SDA_L; 
    delay_n_10us(1);
    SCL_L;
}

//I2C STOP SIGNAL
static void TM1650_Stop(void) 
{
    SDA_Mode_Out();
    SCL_L;
    SDA_L;
    delay_n_10us(1);
    SCL_H;
    delay_n_10us(1);
    SDA_H;
}

//I2C ACK 
static void TM1650_WaitAck(void) 
{
    SDA_Mode_In(); //CHANGE MODE FOR CHECKING SDA LOW
    SCL_H;
    delay_n_10us(1);
    SCL_L;
    SDA_Mode_Out();
}

//WRITE BYTE
static void TM1650_WriteByte(uint8_t dat) 
{
    uint8_t i;
    SDA_Mode_Out();

    for (i = 0; i < 8; i++) 
    {
        SCL_L;  //SCL KEEP LOW
        if (dat & 0x80) // MSB FIST TAKE OUT HIGHEST BIT
            SDA_H;
        else
            SDA_L;
        dat <<= 1;
        delay_n_10us(1);
        SCL_H;  //AFTER DATA READY SCL 0->1
        delay_n_10us(1);
    }
    SCL_L;  //REALEASE
    TM1650_WaitAck();
}

//READ BYTE
static uint8_t TM1650_ReadByte(void) 
{
    uint8_t i, dat = 0;
    SDA_Mode_In();
    for (i = 0; i < 8; i++) 
    {
        dat <<= 1;
        SCL_L;
        delay_n_10us(1);
        SCL_H;
        delay_n_10us(1);
        if (SDA_READ)
            dat |= 0x01;
    }
    SCL_L;
    
    //AFTER READ SEND AN ACK
    SDA_Mode_Out();
    SDA_L;
    delay_n_10us(1);
    SCL_H;
    delay_n_10us(1);
    SCL_L;
    
    return dat;
}

void TM1650_Init(void) 
{
    GPIO_InitType GPIO_InitStructure;
    
    RCC_EnableAPB2PeriphClk(TM1650_GPIO_CLK, ENABLE);
    
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin        = TM1650_PIN_SCL | TM1650_PIN_SDA;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_OUTPUT_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitPeripheral(TM1650_GPIO_PORT, &GPIO_InitStructure);
    
    SCL_H;
    SDA_H;
    
    TM1650_SetDisplay(TM1650_BRIGHT_3, 1);
}

//BRIGHT ENABLE, AND LEVEL (DEFAULT BRIGHT 1 IN INIT)
void TM1650_SetDisplay(uint8_t brightness, uint8_t state) 
{
    uint8_t cmd = brightness;
    if (state) cmd |= 0x01; //KEEP HIGH 4 BIT
    
    TM1650_Start();
    TM1650_WriteByte(TM1650_CMD_MODE); // 0x48 write cmd
    TM1650_WriteByte(cmd);             
    TM1650_Stop();
}

//write display addr
//addr DIG1 - DIG4: 0x68, 0x6A, 0x6C, 0x6E
//every addr 8 bit LSB: A B C D E F G DP
void TM1650_WriteData(uint8_t addr, uint8_t data) 
{
    TM1650_Start();
    TM1650_WriteByte(addr);
    TM1650_WriteByte(data);
    TM1650_Stop();
}

//int display, but we have hex and blank
void TM1650_DisplayInt(uint16_t num)
{
    if(num > 9999) num = 9999;
    
    TM1650_WriteData(TM1650_ADDR_DIG1, TM1650_TubeFont[num / 1000]);
    TM1650_WriteData(TM1650_ADDR_DIG2, (TM1650_TubeFont[(num % 1000) / 100] | 0x80));
    TM1650_WriteData(TM1650_ADDR_DIG3, TM1650_TubeFont[(num % 100) / 10]);
    TM1650_WriteData(TM1650_ADDR_DIG4, TM1650_TubeFont[num % 10]);
}

// 显示 MM:SS 格式（分:秒），并在第二位点亮小数点
void TM1650_DisplayTime(uint16_t total_seconds)
{
    uint8_t minutes, seconds;
    uint8_t min_tens, min_units, sec_tens, sec_units;

    // 1. 安全限制：如果超过 3600秒 (60分钟)，限制在 60:00 或者做归零处理
    // 这里为了演示，我们不做强制截断，支持显示到 99:59
    
    // 2. 核心算法：时间换算
    minutes = total_seconds / 60;  // 算出有多少分钟 (例如 125秒 / 60 = 2分钟)
    seconds = total_seconds % 60;  // 算出剩余多少秒 (例如 125秒 % 60 = 5秒)

    // 3. 拆分分钟的十位和个位
    min_tens  = minutes / 10;
    min_units = minutes % 10;

    // 4. 拆分秒数的十位和个位
    sec_tens  = seconds / 10;
    sec_units = seconds % 10;

    // 5. 写入显存
    // DIG1: 分钟的十位
    TM1650_WriteData(TM1650_ADDR_DIG1, TM1650_TubeFont[min_tens]);
    
    // DIG2: 分钟的个位 + 小数点 (0x80)
    // 注意：这里的 | 0x80 是为了让第二个数码管的小数点亮起，模拟 "分:秒" 的效果
    TM1650_WriteData(TM1650_ADDR_DIG2, TM1650_TubeFont[min_units] | 0x80);
    
    // DIG3: 秒数的十位
    TM1650_WriteData(TM1650_ADDR_DIG3, TM1650_TubeFont[sec_tens]);
    
    // DIG4: 秒数的个位
    TM1650_WriteData(TM1650_ADDR_DIG4, TM1650_TubeFont[sec_units]);
}
