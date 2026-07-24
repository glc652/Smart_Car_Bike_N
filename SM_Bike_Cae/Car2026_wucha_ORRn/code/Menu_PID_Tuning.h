/*
 * Menu_PID_Tuning.h
 * PID�������Բ˵�ϵͳ
 *  Created on: 2026-05-28
 */

#ifndef CODE_MENU_PID_TUNING_H_
#define CODE_MENU_PID_TUNING_H_

#include "zf_common_typedef.h"

typedef enum {
    MENU_MAIN = 0,
    MENU_PID_SELECT = 1,
    MENU_PARAM_EDIT = 2,
    MENU_FLASH_OPERATION = 3,
    MENU_FLASH_STATUS = 4,
    MENU_NAG_OPERATION = 5,
} MenuState;

typedef enum {
    FLASH_OP_MAIN = 0,
    FLASH_OP_SAVE = 1,
    FLASH_OP_LOAD = 2,
    FLASH_OP_ERASE = 3,
    FLASH_OP_STATUS = 4,
} FlashOperation;

void PID_Tuning_Menu_Init(void);
void PID_Tuning_Menu_Show(void);
void PID_Tuning_Menu_Up(void);
void PID_Tuning_Menu_Down(void);
void PID_Tuning_Menu_Select(void);
void PID_Tuning_Menu_Back(void);
void PID_Menu_Key_Process(void);

MenuState PID_Tuning_Menu_GetState(void);

#endif /* CODE_MENU_PID_TUNING_H_ */
