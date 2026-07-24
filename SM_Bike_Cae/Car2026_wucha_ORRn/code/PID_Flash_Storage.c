/*
 * PID_Flash_Storage.c
 * PID参数Flash存储管理实现
 *
 *  Created on: 2026-05-28
 */

#include "PID_Flash_Storage.h"
#include "zf_driver_flash.h"
#include "zf_device_ips200.h"
#include "zf_driver_gpio.h"
#include "stdio.h"
#include "Key.h"
static PID_Flash_Data_TypeDef pid_flash_data;

static uint32_t PID_Flash_Calculate_Checksum(const PID_Flash_Data_TypeDef *data)
{
    uint32_t checksum = 0;
    uint32_t *ptr = (uint32_t *)data;
    uint16_t i;

    // 计算�?2个float的校验和 (不包含checksum字段本身)
    for(i = 0; i < 12; i++) {
        checksum += ptr[i];
    }

    return checksum;
}

// 初始化Flash存储模块
void PID_Flash_Storage_Init(void)
{
    // 检查Flash页是否有数据
    if(flash_check(0, PID_FLASH_PAGE_NUM) == 0) {
        // 页面为空，初始化�?
        flash_buffer_clear();
    }
}

// 保存所有PID参数到Flash
void PID_Flash_Save_All_PID(Incremental_PID_TypeDef *speed_pid,
                            Pid_TypeDef *angle_pid,
                            Pid_TypeDef *gyro_pid,
                            Pid_TypeDef *yaw_pid)
{
    uint16_t i;

    // 填充PID数据结构
    pid_flash_data.speed_kp = speed_pid->Kp;
    pid_flash_data.speed_ki = speed_pid->Ki;
    pid_flash_data.speed_kd = speed_pid->Kd;

    pid_flash_data.angle_kp = angle_pid->Kp;
    pid_flash_data.angle_ki = angle_pid->Ki;
    pid_flash_data.angle_kd = angle_pid->Kd;

    pid_flash_data.gyro_kp = gyro_pid->Kp;
    pid_flash_data.gyro_ki = gyro_pid->Ki;
    pid_flash_data.gyro_kd = gyro_pid->Kd;

    pid_flash_data.yaw_kp = yaw_pid->Kp;
    pid_flash_data.yaw_ki = yaw_pid->Ki;
    pid_flash_data.yaw_kd = yaw_pid->Kd;

        pid_flash_data.checksum = PID_Flash_Calculate_Checksum(&pid_flash_data);

        flash_buffer_clear();

        uint32_t *src = (uint32_t *)&pid_flash_data;
    for(i = 0; i < sizeof(PID_Flash_Data_TypeDef) / sizeof(uint32_t); i++) {
        flash_union_buffer[i].uint32_type = src[i];
    }

    // 写入Flash
    flash_write_page_from_buffer(0, PID_FLASH_PAGE_NUM);
}

// 从Flash加载所有PID参数
uint8_t PID_Flash_Load_All_PID(Incremental_PID_TypeDef *speed_pid,
                               Pid_TypeDef *angle_pid,
                               Pid_TypeDef *gyro_pid,
                               Pid_TypeDef *yaw_pid)
{
    uint16_t i;
    uint32_t calculated_checksum;
// 从Flash读取数据到缓冲区
    flash_read_page_to_buffer(0, PID_FLASH_PAGE_NUM);

    // 将缓冲区数据复制到PID数据结构
    uint32_t *dst = (uint32_t *)&pid_flash_data;
    for(i = 0; i < sizeof(PID_Flash_Data_TypeDef) / sizeof(uint32_t); i++) {
        dst[i] = flash_union_buffer[i].uint32_type;
    }

        calculated_checksum = PID_Flash_Calculate_Checksum(&pid_flash_data);
    if(calculated_checksum != pid_flash_data.checksum) {
        return 0;  // 校验失败
    }

speed_pid->Kp = pid_flash_data.speed_kp;
    speed_pid->Ki = pid_flash_data.speed_ki;
    speed_pid->Kd = pid_flash_data.speed_kd;

    angle_pid->Kp = pid_flash_data.angle_kp;
    angle_pid->Ki = pid_flash_data.angle_ki;
    angle_pid->Kd = pid_flash_data.angle_kd;

    gyro_pid->Kp = pid_flash_data.gyro_kp;
    gyro_pid->Ki = pid_flash_data.gyro_ki;
    gyro_pid->Kd = pid_flash_data.gyro_kd;

    yaw_pid->Kp = pid_flash_data.yaw_kp;
    yaw_pid->Ki = pid_flash_data.yaw_ki;
    yaw_pid->Kd = pid_flash_data.yaw_kd;

    return 1;  // 加载成功
}

// 擦除Flash页面
void PID_Flash_Erase_Page(void)
{
    flash_erase_page(0, PID_FLASH_PAGE_NUM);
    flash_buffer_clear();
}

// 显示Flash存储状�?(用于调试)
void PID_Flash_Show_Status(void)
{
//    char buffer[40];
    uint8_t page_status;
    uint8_t key_pressed = 0;
    uint8_t key_last = 0;

    ips200_clear();
//    ips200_show_string(8*2, 16*0, "PID Flash Status");

        page_status = flash_check(0, PID_FLASH_PAGE_NUM);

    if(page_status == 0) {
        ips200_show_string(8*1, 16*12, "Page: Empty");
    } else {
        ips200_show_string(8*1, 16*12, "Page: Has Data");

        // 读取并显示PID参数
        if(PID_Flash_Load_All_PID((Incremental_PID_TypeDef *)&pid_flash_data,
                                  (Pid_TypeDef *)&pid_flash_data,
                                  (Pid_TypeDef *)&pid_flash_data,
                                  (Pid_TypeDef *)&pid_flash_data) == 1) {
            ips200_show_string(8*1, 16*13, "Checksum: OK");

//            sprintf(buffer, "Speed: %.2f", pid_flash_data.speed_kp);
//            ips200_show_string(8*1, 16*15, buffer);
//            sprintf(buffer, "%.2f", pid_flash_data.speed_ki);
//            ips200_show_string(8*13, 16*15, buffer);
//            sprintf(buffer, "%.2f", pid_flash_data.speed_kd);
//            ips200_show_string(8*18, 16*15, buffer);
//
//            sprintf(buffer, "Angle: %.2f", pid_flash_data.angle_kp);
//            ips200_show_string(8*1, 16*16, buffer);
//            sprintf(buffer, "%.2f", pid_flash_data.angle_ki);
//            ips200_show_string(8*13, 16*16, buffer);
//            sprintf(buffer, "%.2f", pid_flash_data.angle_kd);
//            ips200_show_string(8*18, 16*16, buffer);
//
//            sprintf(buffer, "Gyro: %.2f", pid_flash_data.gyro_kp);
//            ips200_show_string(8*1, 16*17, buffer);
//            sprintf(buffer, "%.2f", pid_flash_data.gyro_ki);
//            ips200_show_string(8*12, 16*17, buffer);
//            sprintf(buffer, "%.2f", pid_flash_data.gyro_kd);
//            ips200_show_string(8*17, 16*17, buffer);
//
//            sprintf(buffer, "Yaw: %.2f", pid_flash_data.yaw_kp);
//            ips200_show_string(8*1, 16*18, buffer);
//            sprintf(buffer, "%.2f", pid_flash_data.yaw_ki);
//            ips200_show_string(8*11, 16*18, buffer);
//            sprintf(buffer, "%.2f", pid_flash_data.yaw_kd);
//            ips200_show_string(8*16, 16*18, buffer);
        } else {
            ips200_show_string(8*1, 16*3, "Checksum: FAIL");
        }
    }

    ips200_show_string(8*1, 16*10, "Press any key");

    // 等待按键返回，不断刷新屏幕防止被覆盖
    while(!key_pressed) {
        uint8_t key1 = (gpio_get_level(KEY1) == GPIO_LOW);
        uint8_t key2 = (gpio_get_level(KEY2) == GPIO_LOW);
        uint8_t key3 = (gpio_get_level(KEY3) == GPIO_LOW);
        uint8_t key4 = (gpio_get_level(KEY4) == GPIO_LOW);
        uint8_t any_key = key1 || key2 || key3 || key4;

                if(any_key && !key_last) {
            key_pressed = 1;
        }
        key_last = any_key;
    }
}
