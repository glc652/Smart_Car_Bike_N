/*
 * PID_Flash_Storage.h
 * PID参数Flash存储管理
 *
 *  Created on: 2026-05-28
 */

#ifndef CODE_PID_FLASH_STORAGE_H_
#define CODE_PID_FLASH_STORAGE_H_

#include "zf_common_typedef.h"
#include "PID.h"

// Flash页号定义 (使用第11页存储PID参数)
#define PID_FLASH_PAGE_NUM          11

// PID参数在Flash中的存储结构
typedef struct {
    float speed_kp;
    float speed_ki;
    float speed_kd;

    float angle_kp;
    float angle_ki;
    float angle_kd;

    float gyro_kp;
    float gyro_ki;
    float gyro_kd;

    float yaw_kp;
    float yaw_ki;
    float yaw_kd;

    uint32_t checksum;  // 校验和，用于验证数据完整性
} PID_Flash_Data_TypeDef;

// 函数声明
void PID_Flash_Storage_Init(void);
void PID_Flash_Save_All_PID(Incremental_PID_TypeDef *speed_pid,Pid_TypeDef *angle_pid,
                            Pid_TypeDef *gyro_pid,
                            Pid_TypeDef *yaw_pid);
uint8_t PID_Flash_Load_All_PID(Incremental_PID_TypeDef *speed_pid,Pid_TypeDef *angle_pid,
                               Pid_TypeDef *gyro_pid,
                               Pid_TypeDef *yaw_pid);
void PID_Flash_Erase_Page(void);
void PID_Flash_Show_Status(void);

#endif /* CODE_PID_FLASH_STORAGE_H_ */
