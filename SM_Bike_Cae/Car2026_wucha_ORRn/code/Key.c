/*
 * Key.c
 *
 *  Created on: 2025年11月13日
 *      Author: 19740
 */
//===================================================Include===================================================
#include "zf_driver_gpio.h"

#include "Menu.h"
#include "Key.h"
//===================================================定义===================================================
#define Key_Sleep_Time 4
//===================================================变量===================================================
uint8 Key_Time[4]= {0,0,0,0}; //用于按键延时
uint8 key_num[4]= {0,0,0,0};//表示四个按键的状态
uint8 key_flag[4] = {0,0,0,0};
//===================================================内部调用===================================================
/*
 * 按键1状态判断，在菜单0中按下表示上移
 */
void key_1(void){
    if(gpio_get_level(KEY1) == GPIO_LOW && key_flag[0]== 0){
        key_flag[0]=1;
    }
    if(gpio_get_level(KEY1) == GPIO_HIGH && key_flag[0]== 1){
        key_flag[0]=0;
        key_num[0]++;
        Menu_NavigateUp();
        if(key_num[0]>=2){
            key_num[0] = 0;
        }
    }
}
/*
 * 按键2状态判断，在菜单1中按下表示下移
 */
void key_2(void){
    if(gpio_get_level(KEY2) == GPIO_LOW && key_flag[1]== 0){
        key_flag[1]=1;
    }
    if(gpio_get_level(KEY2) == GPIO_HIGH && key_flag[1]== 1){
        key_flag[1]=0;
        key_num[1]++;
        Menu_NavigateDown();
        if(key_num[1]>=2){

            key_num[1] = 0;
        }
    }
}
/*
 * 按键3状态判断，在菜单1中按下表示进入子菜单
 */
void key_3(void){
    if(gpio_get_level(KEY3) == GPIO_LOW && key_flag[2]== 0)
    {
        key_flag[2]=1;
    }
    if(gpio_get_level(KEY3) == GPIO_HIGH && key_flag[2]== 1)
    {
        key_flag[2]=0;
        key_num[2]++;
        Menu_Select();
        if(key_num[2]>=2)
        {

            key_num[2] = 0;
        }
    }
}
/*
 * 按键4状态判断,在子菜单返回到母菜单
 */
void key_4(void){
    if(gpio_get_level(KEY4) == GPIO_LOW && key_flag[3]== 0)
    {
        key_flag[3]=1;
    }
    if(gpio_get_level(KEY4) == GPIO_HIGH && key_flag[3]== 1)
    {
        key_flag[3]=0;
        key_num[3]++;
        Menu_Back();
        if(key_num[3]>=2)
        {
            key_num[3] = 0;
        }
    }
}
//===================================================外部调用===================================================
void Key_Init(void){
    gpio_init(KEY1, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY1 输入 默认高电平 上拉输入
    gpio_init(KEY2, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY2 输入 默认高电平 上拉输入
    gpio_init(KEY3, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY3 输入 默认高电平 上拉输入
    gpio_init(KEY4, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY4 输入 默认高电平 上拉输入
}
/*
 * 所有按键的状态判断
 * 放于定时器中断中
 */
void Key_Handler(void)
{
    key_1();
    key_2();
    key_3();
    key_4();
}


