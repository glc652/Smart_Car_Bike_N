/*
 * ps2.c
 * PS2无线手柄驱动
 */

//===================================================Include===================================================
#include "zf_driver_gpio.h"
#include "zf_driver_delay.h"
#include "ps2.h"

//===================================================引脚定义===================================================
#define DATA_Pin    (P10_1)   // 数据输入
#define CMD_Pin     (P10_3)   // 命令输出
#define CS_Pin      (P02_5)    // 片选信号
#define CLK_Pin     (P02_4)    // 时钟信号

// 引脚操作宏定义
#define DATA_read()  gpio_get_level(DATA_Pin)
#define CMD_H()      gpio_set_level(CMD_Pin, 1)
#define CMD_L()      gpio_set_level(CMD_Pin, 0)
#define CS_H()       gpio_set_level(CS_Pin, 1)
#define CS_L()       gpio_set_level(CS_Pin, 0)
#define CLK_H()      gpio_set_level(CLK_Pin, 1)
#define CLK_L()      gpio_set_level(CLK_Pin, 0)

//===================================================常量===================================================
const uint8_t PS2_cmnd[9] = {0x01, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // 查询读取手柄数据
static uint8_t PS2_data[9] = {0}; // 接收的数据缓存

// 手柄状态结构体
static JOYSTICK_TypeDef ps2_current = {0}; // 当前状态
static JOYSTICK_TypeDef ps2_last    = {0}; // 上一帧状态
static JOYSTICK_TypeDef ps2_last_stable = {0};  // 消抖后的稳定状态
static uint8_t ps2_debounce_count = 0;  // 消抖计数器
#define PS2_DEBOUNCE_TIME 3  // 消抖时间，3次读取

//===================================================内部函数===================================================
/**
  * @brief  PS2数据字节读写函数
  * @param  data: 要发送的数据
  * @return 接收的数据字节
  */
static uint8_t PS2_ReadWriteData(uint8_t data)
{
    uint8_t ref, res = 0;

    for(ref = 0x01; ref > 0x00; ref <<= 1)
    {
        CLK_L();          // 时钟拉低
        if(ref & data)
            CMD_H();      // 发送1
        else
            CMD_L();      // 发送0

        system_delay_us(5);
        CLK_H();          // 时钟拉高

        if(DATA_read())   // 读取数据位
            res |= ref;

        system_delay_us(5);
    }
    CMD_H(); // 命令线恢复到高电平

    return res;
}

/**
  * @brief  按键消抖处理
  * @param  无
  * @return 无
  */
static void PS2_Debounce(void)
{
    // 检测状态是否连续相同
    if(ps2_current.btn1 == ps2_last_stable.btn1 && ps2_current.btn2 == ps2_last_stable.btn2)
    {
        ps2_debounce_count = 0;
        return;
    }

    ps2_debounce_count++;
    if(ps2_debounce_count < PS2_DEBOUNCE_TIME)
        return;

    ps2_debounce_count = 0;
    ps2_last_stable = ps2_current;
}

//===================================================外部函数===================================================
/**
  * @brief  PS2初始化
  */
void AX_PS2_Init(void)
{
    gpio_init(DATA_Pin, GPI,  1, GPI_PULL_UP);   // 数据输入 上拉
    gpio_init(CMD_Pin,  GPO,  1, GPO_PUSH_PULL); // 命令输出
    gpio_init(CS_Pin,   GPO,  1, GPO_PUSH_PULL); // 片选输出
    gpio_init(CLK_Pin,  GPO,  1, GPO_PUSH_PULL); // 时钟输出
}

/**
  * @brief  读取PS2手柄按键和摇杆数据
  * @param  *JoystickStruct: 手柄数据结构体指针
  */
void AX_PS2_ScanKey(JOYSTICK_TypeDef *JoystickStruct)
{
    uint8_t i;

    CS_L();                      // 拉低片选开始通信
    system_delay_us(1);

    for(i=0; i<9; i++)
    {
        PS2_data[i] = PS2_ReadWriteData(PS2_cmnd[i]);
        system_delay_us(16);
    }

    CS_H();                      // 拉高片选结束通信

    ps2_last = ps2_current;      // 保存上一帧状态

    // 解析数据到结构体
    JoystickStruct->mode     = PS2_data[1];
    JoystickStruct->btn1     = PS2_data[3];
    JoystickStruct->btn2     = PS2_data[4];
    JoystickStruct->RJoy_LR  = PS2_data[5];  // 右摇杆左右
    JoystickStruct->RJoy_UD  = PS2_data[6];  // 右摇杆上下
    JoystickStruct->LJoy_LR  = PS2_data[7];  // 左摇杆左右
    JoystickStruct->LJoy_UD  = PS2_data[8];  // 左摇杆上下

    ps2_current = *JoystickStruct; // 更新当前状态

    // 执行消抖处理
    PS2_Debounce();
}

/**
  * @brief  获取当前手柄状态指针
  */
JOYSTICK_TypeDef* PS2_GetCurrent(void)
{
    return &ps2_current;
}

/**
  * @brief  获取消抖后的稳定手柄状态指针
  */
JOYSTICK_TypeDef* PS2_GetStable(void)
{
    return &ps2_last_stable;
}

/**
  * @brief  判断按键是否被按下（按下=1）
  */
uint8_t PS2_IsKeyDown(uint8_t btn_byte, uint8_t btn_bit)
{
    return !(btn_byte & btn_bit);
}

/**
  * @brief  判断按键是否刚被按下（从未按到按下）
  */
uint8_t PS2_IsKeyPressed_Edge(uint8_t btn_byte, uint8_t last_btn_byte, uint8_t btn_bit)
{
    return (!(btn_byte & btn_bit)) && (last_btn_byte & btn_bit);
}

/**
  * @brief  摇杆值映射：0-255 -> -100 ~ 100
  */
int8_t PS2_GetJoyValue(uint8_t joy_value)
{
    int16_t temp = (int16_t)joy_value - 128;
    temp = (temp * 100) / 128;

    if(temp >  100) temp = 100;
    if(temp < -100) temp = -100;

    return (int8_t)temp;
}
