/*
 * Menu_Integration.c
 * 将PID调参菜单集成到现有菜单系统中
 *
 * 本文件展示如何将PID调参功能添加到你现有的Menu.c中
 */
#include "zf_common_headfile.h"
#include "zf_device_ips200.h"
#include "zf_common_font.h"
#include "Car_control.h"
#include "stdio.h"

//extern Incremental_PID_TypeDef Speed_PID;
//extern Pid_TypeDef Gyro_PID;
//extern Pid_TypeDef Angle_PID;
//extern Pid_TypeDef Yaw_PID;

// 在你的Menu.c中添加这些变量
typedef struct {
    uint8_t in_pid_menu;
    uint8_t pid_select;      // 0=Speed, 1=Angle, 2=Gyro, 3=Yaw
    uint8_t param_select;    // 0=Kp, 1=Ki, 2=Kd
    uint8_t editing;
    float step;
} PID_Menu_State;

static PID_Menu_State pid_menu = {
    .in_pid_menu = 0,
    .pid_select = 0,
    .param_select = 0,
    .editing = 0,
    .step = 0.1f,
};

static const char* pid_names[] = {"Speed", "Angle", "Gyro", "Yaw"};
static const char* param_names[] = {"Kp", "Ki", "Kd"};

static float* get_pid_param(uint8_t pid, uint8_t param)
{
    switch(pid) {
        case 0: // Speed
            if(param == 0) return &Speed_PID.Kp;
            if(param == 1) return &Speed_PID.Ki;
            if(param == 2) return &Speed_PID.Kd;
            break;
        case 1: // Angle
            if(param == 0) return &Angle_PID.Kp;
            if(param == 1) return &Angle_PID.Ki;
            if(param == 2) return &Angle_PID.Kd;
            break;
        case 2: // Gyro
            if(param == 0) return &Gyro_PID.Kp;
            if(param == 1) return &Gyro_PID.Ki;
            if(param == 2) return &Gyro_PID.Kd;
            break;
        case 3: // Yaw
            if(param == 0) return &Yaw_PID.Kp;
            if(param == 1) return &Yaw_PID.Ki;
            if(param == 2) return &Yaw_PID.Kd;
            break;
    }
    return NULL;
}

// 在你的Menu.c中的Menu_Page0函数中添加PID菜单选项
void Menu_Page0_With_PID(void)
{
    ips200_show_string(8*5, 16*0, "Menu");
    ips200_show_string(8*0, 16*1, " ");
    ips200_show_uint(8*11, 16*0, 0, 2);  // 编队数量
    ips200_show_string(8*1, 16*1, "Data:");
    ips200_show_string(8*0, 16*2, " ");
    ips200_show_string(8*1, 16*2, "Kemu1");
    ips200_show_string(8*0, 16*3, " ");
    ips200_show_string(8*1, 16*3, "Kemu2");
    ips200_show_string(8*0, 16*4, " ");
    ips200_show_string(8*1, 16*4, "Kemu3");
    ips200_show_string(8*0, 16*5, " ");
    ips200_show_string(8*1, 16*5, "Kemu4");
    ips200_show_string(8*0, 16*6, " ");
    ips200_show_string(8*1, 16*6, "PID Tuning");  // 新增PID调参选项
}

// PID菜单显示函数
void Menu_Show_PID_Tuning(void)
{
    char buffer[40];

    ips200_clear();

    if(!pid_menu.editing) {
        // 选择PID阶段
        ips200_show_string(8*4, 16*0, "Select PID");

        for(uint8_t i = 0; i < 4; i++) {
            if(i == pid_menu.pid_select) {
                ips200_show_string(8*1, 16*(2+i), ">");
            } else {
                ips200_show_string(8*1, 16*(2+i), " ");
            }

            float* kp = get_pid_param(i, 0);
            if(kp) {
                sprintf(buffer, "%s: Kp=%.2f", pid_names[i], *kp);
                ips200_show_string(8*3, 16*(2+i), buffer);
            }
        }

        ips200_show_string(8*1, 16*7, "SELECT to edit");
        ips200_show_string(8*1, 16*8, "BACK to exit");
    }
    else {
        // 编辑参数阶段
        sprintf(buffer, "Edit %s", pid_names[pid_menu.pid_select]);
        ips200_show_string(8*3, 16*0, buffer);

        for(uint8_t i = 0; i < 3; i++) {
            if(i == pid_menu.param_select) {
                ips200_show_string(8*1, 16*(2+i), ">");
            } else {
                ips200_show_string(8*1, 16*(2+i), " ");
            }

            float* param = get_pid_param(pid_menu.pid_select, i);
            if(param) {
                sprintf(buffer, "%s: %.3f", param_names[i], *param);
                ips200_show_string(8*3, 16*(2+i), buffer);
            }
        }

        sprintf(buffer, "Step: %.2f", pid_menu.step);
        ips200_show_string(8*1, 16*6, buffer);

        ips200_show_string(8*1, 16*8, "LEFT/RIGHT: adjust");
        ips200_show_string(8*1, 16*9, "UP/DOWN: step");
        ips200_show_string(8*1, 16*10, "BACK: done");
    }
}

// 在你的Menu_NavigateUp中添加
void Menu_NavigateUp_PID(void)
{
    if(!pid_menu.editing) {
        if(pid_menu.pid_select > 0) {
            pid_menu.pid_select--;
        } else {
            pid_menu.pid_select = 3;
        }
    } else {
        if(pid_menu.step < 10.0f) {
            pid_menu.step *= 2.0f;
        }
    }
}

// 在你的Menu_NavigateDown中添加
void Menu_NavigateDown_PID(void)
{
    if(!pid_menu.editing) {
        if(pid_menu.pid_select < 3) {
            pid_menu.pid_select++;
        } else {
            pid_menu.pid_select = 0;
        }
    } else {
        if(pid_menu.step > 0.01f) {
            pid_menu.step /= 2.0f;
        }
    }
}

// 处理LEFT按键
void Menu_Left_PID(void)
{
    if(pid_menu.editing) {
        float* param = get_pid_param(pid_menu.pid_select, pid_menu.param_select);
        if(param) {
            *param -= pid_menu.step;
            if(*param < 0) *param = 0;
        }
    }
}

// 处理RIGHT按键
void Menu_Right_PID(void)
{
    if(pid_menu.editing) {
        float* param = get_pid_param(pid_menu.pid_select, pid_menu.param_select);
        if(param) {
            *param += pid_menu.step;
        }
    }
}

// 在你的Menu_Select中添加
void Menu_Select_PID(void)
{
    if(!pid_menu.editing) {
        pid_menu.editing = 1;
        pid_menu.param_select = 0;
        pid_menu.step = 0.1f;
    } else {
        // 在参数之间切换
        if(pid_menu.param_select < 2) {
            pid_menu.param_select++;
        } else {
            pid_menu.param_select = 0;
        }
    }
}

// 在你的Menu_Back中添加
void Menu_Back_PID(void)
{
    if(pid_menu.editing) {
        pid_menu.editing = 0;
    } else {
        pid_menu.in_pid_menu = 0;
    }
}

// 获取PID菜单状态
uint8_t PID_Menu_IsActive(void)
{
    return pid_menu.in_pid_menu;
}

// 进入PID菜单
void PID_Menu_Enter(void)
{
    pid_menu.in_pid_menu = 1;
    pid_menu.pid_select = 0;
    pid_menu.param_select = 0;
    pid_menu.editing = 0;
    pid_menu.step = 0.1f;
}

/*
 * 集成说明：
 *
 * 1. 在你的Menu.c中包含本文件或复制相关函数
 *
 * 2. 在Menu_Page0中添加PID调参选项：
 *    ips200_show_string(8*1, 16*6, "PID Tuning");
 *
 * 3. 在Menu_Select中添加：
 *    if(mainmenu.set == 6) {  // PID Tuning选项
 *        PID_Menu_Enter();
 *    }
 *
 * 4. 在主菜单显示循环中添加：
 *    if(PID_Menu_IsActive()) {
 *        Menu_Show_PID_Tuning();
 *    } else {
 *        Menu_show();
 *    }
 *
 * 5. 在按键处理中添加：
 *    if(PID_Menu_IsActive()) {
 *        if(key_up) Menu_NavigateUp_PID();
 *        if(key_down) Menu_NavigateDown_PID();
 *        if(key_left) Menu_Left_PID();
 *        if(key_right) Menu_Right_PID();
 *        if(key_select) Menu_Select_PID();
 *        if(key_back) Menu_Back_PID();
 *    }
 */
