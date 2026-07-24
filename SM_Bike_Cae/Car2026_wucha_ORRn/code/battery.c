/*
 * battery.c
 *
 *  Created on: 2026年2月22日
 *      Author: 19740
 */
//===================================================Include===================================================
#include "zf_driver_adc.h"
#include "zf_device_ips200.h"
#include "Menu.h"
//===================================================Define&Declare===================================================
#define VOLTAGE_PORT    ADC0_CH11_A11 // 定义检测端口
uint16 valtage_adc = 0;
float valtage_now = 0;
//===================================================内部调用===================================================
//===================================================外部调用===================================================
void battery_init(void){
    adc_init(VOLTAGE_PORT, ADC_12BIT);                                          // 建议电磁传感器用 ADC1 电池检测用 ADC2
}

void get_battery(void){
    valtage_adc = adc_convert(VOLTAGE_PORT);
    valtage_now = 36.3 * valtage_adc / 4096;                                // 根据逐飞科技学习主板分压计算 测得应该是电池电压 1/4 那么最大量程 36.3V
    if (mainmenu.set == 1){
        ips200_show_string(Row_1, Line_16, "Battery Valtage:");
        ips200_show_float(Row_16, Line_16, valtage_now, 2, 3);
    }

}



