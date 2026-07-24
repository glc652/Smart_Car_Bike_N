/*
 * LCD.h
 *
 *  Created on: 2025年11月13日
 *      Author: glc
 */

#ifndef CODE_MENU_H_
#define CODE_MENU_H_

#include "stdbool.h"
#include "zf_common_typedef.h"
//行的宏定义
#define Line_0 (16*0)//第零行
#define Line_1 (16*1)//第一行
#define Line_2 (16*2)//第二行
#define Line_3 (16*3)//第三行
#define Line_4 (16*4)//第四行
#define Line_5 (16*5)//第五行
#define Line_6 (16*6)//第六行
#define Line_7 (16*7)//第七行
#define Line_8 (16*8)//第八行
#define Line_9 (16*9)//第九行
//以下为isp200需要
#define Line_10 (16*10)
#define Line_11 (16*11)
#define Line_12 (16*12)
#define Line_13 (16*13)
#define Line_14 (16*14)
#define Line_15 (16*15)
#define Line_16 (16*16)

//列的宏定义
#define Row_0 (8*0)//第零列
#define Row_1 (8*1)//第一列
#define Row_2 (8*2)//第二列
#define Row_3 (8*3)//第三列
#define Row_4 (8*4)//第四列
#define Row_5 (8*5)//第五列
#define Row_6 (8*6)//第六列
#define Row_7 (8*7)//第七列
#define Row_8 (8*8)//第八列
#define Row_9 (8*9)//第九列
#define Row_10 (8*10)//第十列
#define Row_11 (8*11)//第十一列
#define Row_12 (8*12)//第十二列
#define Row_13 (8*13)//第十三列
#define Row_14 (8*14)//第十四列
#define Row_15 (8*15)//第十五列
#define Row_16 (8*16)//第十六列

typedef enum
{
    Page0 = 0,
    Page1 = 1,
    Page2 = 2,
    Page3 = 3,
    Page4 = 4,
    Page5 = 5,
}Menu_Handle;
// 定义菜单项的函数类型
typedef void (*MenuFunction)(void);

// 菜单项结构体
typedef struct MenuItem {
    const char* name;               // 菜单项名称
    MenuFunction function;          // 菜单项对应的功能函数
    struct MenuItem* parent;        // 指向父菜单的指针
    struct MenuItem* down;          // 指向下一个菜单项的指针
    struct MenuItem* up;
    struct MenuItem* children;      // 指向子菜单的指针（如果有）
    bool FirstEnterFunction;        // 第一次执行对应Item的任务函数
    uint8_t  set;
} MenuItem;

extern MenuItem mainmenu;
extern uint8 qidong;
void Menu_Init(void);
// 导航菜单
void Menu_NavigateUp(void );
void Menu_NavigateDown(void );
void Menu_Select(void );
void Menu_Back(void );
void Menu_show(void);
#endif /* CODE_MENU_H_ */
