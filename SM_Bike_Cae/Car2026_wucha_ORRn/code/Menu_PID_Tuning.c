/*
 * Menu_PID_Tuning.c
 * PID参数调试菜单系统
 *
 *  Created on: 2026-05-28
 */
//===================================================Include===================================================
#include "zf_device_ips200.h"
#include "zf_common_font.h"
#include "zf_driver_gpio.h"
#include "Car_control.h"
#include "Key.h"
#include "stdio.h"
#include "PID_Flash_Storage.h"
#include "Menu_PID_Tuning.h"
#include "navigation.h"
#include "zf_common_headfile.h"
#include "Ins.h"
//===================================================宏定义&全局变量===================================================
// 外部PID结构体引用
extern Incremental_PID_TypeDef Speed_PID;
extern Pid_TypeDef Gyro_PID;
extern Pid_TypeDef Angle_PID;
extern Pid_TypeDef Yaw_PID;

// PID类型
typedef enum {
    PID_SPEED = 0,    // 速度环
    PID_ANGLE = 1,    // 角度环
    PID_GYRO  = 2,    // 角速度环
    PID_YAW   = 3,    // 偏航环
} PID_Type;

// PID参数
typedef enum {
    PARAM_KP = 0,
    PARAM_KI = 1,
    PARAM_KD = 2,
} PID_Param;

// 菜单控制结构
typedef struct {
    MenuState state;
    uint8_t current_pid;
    PID_Param current_param;
    uint8_t editing;
    float step;
    float edit_value;
    FlashOperation flash_op;  // Flash操作状态
    uint8_t flash_op_result;  // Flash操作结果
} MenuContext;

static MenuContext ctx = {
    .state = MENU_MAIN,
    .current_pid = 6,
    .current_param = PARAM_KP,
    .editing = 0,
    .step = 0.1f,
    .edit_value = 0.0f,
};

static uint8_t menu_need_refresh = 1;  // 屏幕刷新标志

static const char* pid_names[] = {"Speed", "Angle", "Gyro", "Yaw"};
static const char* param_names[] = {"Kp", "Ki", "Kd"};

static void menu_start_replay(void)
{
    if(Nag_Load_Flash_To_Array()){
        N.Nag_SystemRun_Index = 2;
        N.Mileage_All = 0;
        N.Nag_Stop_f = 0;
        N.Save_count = 0;
        ctx.flash_op_result = 1;
    }else{
        printf("Load ERROR!\n");
        ctx.flash_op_result = 2;
    }
}

// 获取对应PID参数指针
static float* get_param_pointer(PID_Type pid, PID_Param param)
{
    switch(pid) {
        case PID_SPEED:
            if(param == PARAM_KP) return &Speed_PID.Kp;
            if(param == PARAM_KI) return &Speed_PID.Ki;
            if(param == PARAM_KD) return &Speed_PID.Kd;
            break;
        case PID_ANGLE:
            if(param == PARAM_KP) return &Angle_PID.Kp;
            if(param == PARAM_KI) return &Angle_PID.Ki;
            if(param == PARAM_KD) return &Angle_PID.Kd;
            break;
        case PID_GYRO:
            if(param == PARAM_KP) return &Gyro_PID.Kp;
            if(param == PARAM_KI) return &Gyro_PID.Ki;
            if(param == PARAM_KD) return &Gyro_PID.Kd;
            break;
        case PID_YAW:
            if(param == PARAM_KP) return &Yaw_PID.Kp;
            if(param == PARAM_KI) return &Yaw_PID.Ki;
            if(param == PARAM_KD) return &Yaw_PID.Kd;
            break;
        default:
            break;
    }
    return NULL;
}

// 菜单初始化
void PID_Tuning_Menu_Init(void)
{
    ctx.state = MENU_MAIN;
    ctx.current_pid = 6;
    ctx.current_param = PARAM_KP;
    ctx.editing = 0;
    ctx.step = 0.1f;
    menu_need_refresh = 1;
}

// 菜单显示函数
void PID_Tuning_Menu_Show(void)
{
    char buffer[40];

    // 需要刷新时清屏
    if(menu_need_refresh) {
        ips200_clear();
        menu_need_refresh = 0;
    }

    if(ctx.state == MENU_MAIN) {
        ips200_show_string(8*5, 16*0, "PID Tuning");
        ips200_show_string(8*1, 16*2, "Select:");

        ips200_show_string(8*1, 16*3, (ctx.current_pid == 6) ? ">" : " ");
        ips200_show_string(8*3, 16*3, "Start");

        for(uint8_t i = 0; i < 4; i++) {
            ips200_show_string(8*1, 16*(4+i), (i == ctx.current_pid) ? ">" : " ");
            ips200_show_string(8*3, 16*(4+i), (char*)pid_names[i]);
        }
        ips200_show_string(8*1, 16*8, (ctx.current_pid == 4) ? ">" : " ");
        ips200_show_string(8*3, 16*8, "Flash");
        ips200_show_string(8*1, 16*9, (ctx.current_pid == 5) ? ">" : " ");
        ips200_show_string(8*3, 16*9, "Navigation");

        if(ctx.flash_op_result == 1) {
            ips200_show_string(8*1, 16*10, "Start: Success");
        } else if(ctx.flash_op_result == 2) {
            ips200_show_string(8*1, 16*10, "Start: Failed");
        }

        ips200_show_string(8*1, 16*11, "SELECT: enter");
        ips200_show_string(8*1, 16*12, "BACK  : exit");
    }
    else if(ctx.state == MENU_PID_SELECT) {
        sprintf(buffer, "%s PID Parameters", pid_names[ctx.current_pid]);
        ips200_show_string(8*2, 16*0, buffer);
        ips200_show_string(8*1, 16*2, "Select Parameter:");

        for(uint8_t i = 0; i < 3; i++) {
            ips200_show_string(8*1, 16*(3+i), (i == ctx.current_param) ? ">" : " ");

            float* param_ptr = get_param_pointer(ctx.current_pid, i);
            if(param_ptr) {
                sprintf(buffer, "%s: %.5f", param_names[i], *param_ptr);
                ips200_show_string(8*3, 16*(3+i), buffer);
            }
        }
        ips200_show_string(8*1, 16*7, "SELECT: edit");
        ips200_show_string(8*1, 16*8, "BACK  : return");
    }
    else if(ctx.state == MENU_PARAM_EDIT) {
        sprintf(buffer, "Edit %s-%s", pid_names[ctx.current_pid], param_names[ctx.current_param]);
        ips200_show_string(8*0, 16*0, buffer);

        float* param_ptr = get_param_pointer(ctx.current_pid, ctx.current_param);
        if(param_ptr) {
            sprintf(buffer, "Now: %.5f", *param_ptr);
            ips200_show_string(8*1, 16*2, buffer);

            sprintf(buffer, "Step: %.5f", ctx.step);
            ips200_show_string(8*1, 16*3, buffer);

            ips200_show_string(8*1, 16*5, "UP/DOWN: step");
            ips200_show_string(8*1, 16*6, "SELECT: + step");
            ips200_show_string(8*1, 16*7, "BACK  : - step");
            ips200_show_string(8*1, 16*9, "Long BACK: return");
        }
    }
    else if(ctx.state == MENU_FLASH_OPERATION) {
            ips200_show_string(8*4, 16*0, "Flash Operation");
            ips200_show_string(8*1, 16*2, "Select:");

            ips200_show_string(8*1, 16*3, (ctx.flash_op == FLASH_OP_SAVE) ? ">" : " ");
            ips200_show_string(8*3, 16*3, "Save PID");

            ips200_show_string(8*1, 16*4, (ctx.flash_op == FLASH_OP_LOAD) ? ">" : " ");
            ips200_show_string(8*3, 16*4, "Load PID");

            ips200_show_string(8*1, 16*5, (ctx.flash_op == FLASH_OP_ERASE) ? ">" : " ");
            ips200_show_string(8*3, 16*5, "Erase Flash");

            ips200_show_string(8*1, 16*6, (ctx.flash_op == FLASH_OP_STATUS) ? ">" : " ");
            ips200_show_string(8*3, 16*6, "View Status");

            // 显示上一次操作结果
            if(ctx.flash_op_result == 1) {
                ips200_show_string(8*1, 16*8, "Last: Success");
            } else if(ctx.flash_op_result == 2) {
                ips200_show_string(8*1, 16*8, "Last: Failed");
            }

            ips200_show_string(8*1, 16*10, "SELECT: execute");
        }
    else if(ctx.state == MENU_NAG_OPERATION) {
        char nag_buf[32];
        Nag_State_t nag_st = Navigation_Get_State();

        ips200_show_string(8*3, 16*0, "Navigation");

        // 状态显示
        const char* st_str = "IDLE";
        if(nag_st == NAG_STATE_RECORD) st_str = "RECORDING";
        else if(nag_st == NAG_STATE_REPLAY) st_str = "REPLAYING";
        ips200_show_string(8*1, 16*1, "State:");
        ips200_show_string(8*8, 16*1, (char*)st_str);

        // 已记录点数 + 距离
        sprintf(nag_buf, "Points:%5d", Navigation_Get_Total_Points());
        ips200_show_string(8*1, 16*2, nag_buf);
        sprintf(nag_buf, "Dist: %.2fm", Navigation_Get_Total_Points() * NAG_SAMPLE_DIST);
        ips200_show_string(8*1, 16*3, nag_buf);

        // 实时偏航角
        sprintf(nag_buf, "Yaw:  %7.2f", Navigation_Get_Current_Yaw());
        ips200_show_string(8*1, 16*4, nag_buf);

        // 操作选项
        ips200_show_string(8*1, 16*6, (ctx.flash_op == 0) ? ">" : " ");
        ips200_show_string(8*3, 16*6, "Start Record");
        ips200_show_string(8*1, 16*7, (ctx.flash_op == 1) ? ">" : " ");
        ips200_show_string(8*3, 16*7, "Stop  Record");
        ips200_show_string(8*1, 16*8, (ctx.flash_op == 2) ? ">" : " ");
        ips200_show_string(8*3, 16*8, "Start Replay");
        ips200_show_string(8*1, 16*9, (ctx.flash_op == 3) ? ">" : " ");
        ips200_show_string(8*3, 16*9, "Stop  Replay");
        ips200_show_string(8*1, 16*10, (ctx.flash_op == 4) ? ">" : " ");
        ips200_show_string(8*3, 16*10, "Erase Data");

        ips200_show_string(8*1, 16*12, "SELECT:exec BACK:ret");
    }
}

void PID_Tuning_Menu_Up(void)
{
    if(ctx.state == MENU_MAIN) {
        if(ctx.current_pid > 0) {
            ctx.current_pid--;
        } else {
            ctx.current_pid = 6;
        }
        menu_need_refresh = 1;
    }
    else if(ctx.state == MENU_PID_SELECT) {
        ctx.current_param = (ctx.current_param > 0) ? (ctx.current_param - 1) : 2;
        menu_need_refresh = 1;
    }
    else if(ctx.state == MENU_PARAM_EDIT) {
        if(ctx.step < 10.0f) ctx.step *= 2.0f;
        menu_need_refresh = 1;
    }
    else if(ctx.state == MENU_FLASH_OPERATION) {
        ctx.flash_op = (ctx.flash_op > 0) ? (ctx.flash_op - 1) : FLASH_OP_STATUS;
        menu_need_refresh = 1;
    }
    else if(ctx.state == MENU_NAG_OPERATION) {
        ctx.flash_op = (ctx.flash_op > 0) ? (ctx.flash_op - 1) : 4;
        menu_need_refresh = 1;
    }
}

void PID_Tuning_Menu_Down(void)
{
    if(ctx.state == MENU_MAIN) {
        if(ctx.current_pid < 6) {
            ctx.current_pid++;
        } else {
            ctx.current_pid = 0;
        }
        menu_need_refresh = 1;
    }
    else if(ctx.state == MENU_PID_SELECT) {
        ctx.current_param = (ctx.current_param < 2) ? (ctx.current_param + 1) : 0;
        menu_need_refresh = 1;
    }
    else if(ctx.state == MENU_PARAM_EDIT) {
        if(ctx.step > 0.00001f){
            ctx.step /= 10.0f;
        }else{
            ctx.step = 0.00001f;
        }
        menu_need_refresh = 1;
    }
    else if(ctx.state == MENU_FLASH_OPERATION) {
        ctx.flash_op = (ctx.flash_op < FLASH_OP_STATUS) ? (ctx.flash_op + 1) : FLASH_OP_SAVE;
        menu_need_refresh = 1;
    }
    else if(ctx.state == MENU_NAG_OPERATION) {
        ctx.flash_op = (ctx.flash_op < 4) ? (ctx.flash_op + 1) : 0;
        menu_need_refresh = 1;
    }
}

// SELECT按键处理
void PID_Tuning_Menu_Select(void)
{
    if(ctx.state == MENU_MAIN) {
        if(ctx.current_pid == 6) {
            menu_start_replay();
        } else if(ctx.current_pid < 4) {
            ctx.state = MENU_PID_SELECT;
            ctx.current_param = PARAM_KP;
        } else if(ctx.current_pid == 4) {
            ctx.state = MENU_FLASH_OPERATION;
            ctx.flash_op = FLASH_OP_SAVE;
            ctx.flash_op_result = 0;
        } else if(ctx.current_pid == 5) {
            ctx.state = MENU_NAG_OPERATION;
            ctx.flash_op = 0;
        }
        menu_need_refresh = 1;
    }
    else if(ctx.state == MENU_PID_SELECT) {
        ctx.state = MENU_PARAM_EDIT;
        ctx.step = 0.1f;
        menu_need_refresh = 1;
    }
    else if(ctx.state == MENU_PARAM_EDIT) {
        float* param_ptr = get_param_pointer(ctx.current_pid, ctx.current_param);
        if(param_ptr) *param_ptr += ctx.step;
        menu_need_refresh = 1;
    }
    else if(ctx.state == MENU_FLASH_OPERATION) {
        // 执行Flash操作
        switch(ctx.flash_op) {
            case FLASH_OP_SAVE:
                PID_Flash_Save_All_PID(&Speed_PID, &Angle_PID, &Gyro_PID, &Yaw_PID);
                ctx.flash_op_result = 1;
                break;
            case FLASH_OP_LOAD:
                if(PID_Flash_Load_All_PID(&Speed_PID, &Angle_PID, &Gyro_PID, &Yaw_PID) == 1) {
                    ctx.flash_op_result = 1;
                } else {
                    ctx.flash_op_result = 2;
                }
                break;
            case FLASH_OP_ERASE:
                PID_Flash_Erase_Page();
                ctx.flash_op_result = 1;
                break;
            case FLASH_OP_STATUS:
                PID_Flash_Show_Status();
                break;
            default:
                break;
        }
        menu_need_refresh = 1;
    }
    else if(ctx.state == MENU_NAG_OPERATION) {
        // 执行导航操作
        switch(ctx.flash_op) {
            case 0: Navigation_Start_Record(); break;
            case 1: Navigation_Stop_Record();  break;
            case 2: Navigation_Start_Replay(); break;
            case 3: Navigation_Stop_Replay();  break;
            case 4: Navigation_Erase_All();    break;
            default: break;
        }
        menu_need_refresh = 1;
    }
}

// 短按BACK: -STEP / 返回上一级
void PID_Tuning_Menu_Back(void)
{
    if(ctx.state == MENU_PARAM_EDIT) {
        float* param_ptr = get_param_pointer(ctx.current_pid, ctx.current_param);
        if(param_ptr) {
            *param_ptr -= ctx.step;
//            if(*param_ptr < 0) *param_ptr = 0;
        }
    }
    else if(ctx.state == MENU_PID_SELECT) {
        ctx.state = MENU_MAIN;
    }
    else if(ctx.state == MENU_FLASH_OPERATION) {
        ctx.state = MENU_MAIN;
    }
    else if(ctx.state == MENU_NAG_OPERATION) {
        ctx.state = MENU_MAIN;
    }
    menu_need_refresh = 1;
}

// 长按BACK: 强制返回上一级菜单
void PID_Tuning_Menu_Back_Long(void)
{
    if(ctx.state == MENU_PARAM_EDIT) {
        ctx.state = MENU_PID_SELECT;
    }
    else if(ctx.state == MENU_PID_SELECT) {
        ctx.state = MENU_MAIN;
    }
    else if(ctx.state == MENU_NAG_OPERATION) {
        ctx.state = MENU_MAIN;
    }
    menu_need_refresh = 1;
}

// 获取当前菜单状态
MenuState PID_Tuning_Menu_GetState(void)
{
    return ctx.state;
}

//============================ 按键处理模块 ================================

typedef struct {
    uint8_t key_up_last;
    uint8_t key_down_last;
    uint8_t key_select_last;
    uint8_t key_back_last;
    uint16_t back_hold_cnt;
} KeyState;

static KeyState key_state = {0};

// 按键消抖
static uint8_t key_debounce(uint8_t current, uint8_t* last)
{
    if(current && !(*last)) {
        *last = 1;
        return 1;
    }
    if(!current && *last) {
        *last = 0;
    }
    return 0;
}

// 菜单按键处理 建议10ms调用一次
void PID_Menu_Key_Process(void)
{
    uint8_t key_up    = (gpio_get_level(KEY1) == GPIO_LOW);
    uint8_t key_down  = (gpio_get_level(KEY2) == GPIO_LOW);
    uint8_t key_sel   = (gpio_get_level(KEY3) == GPIO_LOW);
    uint8_t key_back  = (gpio_get_level(KEY4) == GPIO_LOW);

    // UP
    if(key_debounce(key_up, &key_state.key_up_last)) {
        PID_Tuning_Menu_Up();
    }
    // DOWN
    if(key_debounce(key_down, &key_state.key_down_last)) {
        PID_Tuning_Menu_Down();
    }
    // SELECT
    if(key_debounce(key_sel, &key_state.key_select_last)) {
        PID_Tuning_Menu_Select();
    }

    // BACK 长按/短按判断
    if(key_back) {
        key_state.back_hold_cnt++;
        // 长按500ms触发长按
        if(key_state.back_hold_cnt == 50) {
            PID_Tuning_Menu_Back_Long();
        }
    } else {
        // 松开时判断短按
        if(key_state.back_hold_cnt > 0 && key_state.back_hold_cnt < 50) {
            PID_Tuning_Menu_Back();
        }
        key_state.back_hold_cnt = 0;
        key_state.key_back_last = 0;
    }
}
