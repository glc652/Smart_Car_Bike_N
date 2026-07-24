/*
 * LCD.c
 *
 *  Created on: 2025锟斤拷11锟斤拷13锟斤拷
 *      Author: 19740
 */
//===================================================Include===================================================
#include "zf_common_font.h"
#include "stdio.h"
#include "zf_common_headfile.h"

//===================================================锟斤拷锟斤拷===================================================
MenuItem mainmenu;                                           // 锟剿碉拷锟结构锟斤拷
MenuItem submenu1;
MenuItem submenu2;
MenuItem submenu3;
MenuItem submenu4;
MenuItem submenu5;
MenuItem submenu1_1;
MenuItem submenu2_1;
MenuItem submenu3_1;
MenuItem submenu4_1;
MenuItem submenu5_1;

uint8_t Arrow_line = 1; //锟斤拷头指锟斤拷锟斤拷锟斤拷
uint8_t Page_num = 0; //锟剿碉拷锟斤拷前页

uint8 qidong =0;
//===================================================锟节诧拷锟斤拷锟斤拷===================================================
void Menu_Page0(void);
void Menu_Page1(void);
void Menu_Page2(void);
void Menu_Page3(void);
void Menu_Page4(void);
void Menu_Page5(void);

/*
 * 锟剿碉拷锟结构锟斤拷锟绞硷拷锟�
 * item-锟剿碉拷锟斤拷指锟诫；name-锟剿碉拷锟斤拷锟斤拷锟狡ｏ拷MenuFunction-锟剿碉拷选锟斤拷时执锟叫的回碉拷锟斤拷锟斤拷锟斤拷id-锟剿碉拷锟斤拷
 */
void Menu_SetFunction(MenuItem* item, const char* name, void (*MenuFunction),uint8_t id)
{
    item->name = name;
    item->function = MenuFunction;
    item->children = NULL;
    item->down = NULL;
    item->parent = NULL;
    item->set = id;
    item->up = NULL;
    item->FirstEnterFunction = false;
}

/*
 * 锟斤拷锟斤拷锟剿碉拷锟斤拷锟斤拷锟斤拷锟结构
 */
void link_menu(void)//锟斤拷锟斤拷锟剿碉拷系统
{
    Menu_SetFunction(&submenu1, "Function 1", Menu_Page0, 1);
    Menu_SetFunction(&submenu2, "Function 2", Menu_Page0, 2);
    Menu_SetFunction(&submenu3, "Function 3", Menu_Page0, 3);
    Menu_SetFunction(&submenu4, "Function 4", Menu_Page0, 4);
    Menu_SetFunction(&submenu5, "Function 5", Menu_Page0, 5);
    Menu_SetFunction(&submenu1_1, "Function 1_1", Menu_Page1, 6);
    Menu_SetFunction(&submenu2_1, "Function 2_1", Menu_Page2, 7);
    Menu_SetFunction(&submenu3_1, "Function 3_1", Menu_Page3, 8);
    Menu_SetFunction(&submenu4_1, "Function 4_1", Menu_Page4, 9);
    Menu_SetFunction(&submenu5_1, "Function 5_1", Menu_Page5, 10);

    // 锟斤拷锟接菜碉拷锟斤拷为锟斤拷锟斤拷
    // 锟斤拷锟斤拷循锟斤拷锟斤拷锟斤拷锟斤拷down指锟诫）
    submenu1.down = &submenu2;
    submenu2.down = &submenu3;
    submenu3.down = &submenu4;
    submenu4.down = &submenu5;
    submenu5.down = &submenu1;//锟轿成闭伙拷
    // 锟斤拷锟津导猴拷锟斤拷锟斤拷锟斤拷up指锟诫）
    submenu1.up = &submenu1;
    submenu2.up = &submenu1;
    submenu3.up = &submenu2;
    submenu4.up = &submenu3;
    submenu5.up = &submenu4;
    // 锟斤拷锟剿碉拷锟斤拷母锟斤拷锟斤拷锟较碉拷锟斤拷锟街革拷锟絪ubmenu1锟斤拷
    submenu1.parent = &submenu1;
    submenu2.parent = &submenu1;
    submenu3.parent = &submenu1;
    submenu4.parent = &submenu1;
    submenu5.parent = &submenu1;
    // 锟斤拷锟剿碉拷锟斤拷指锟斤拷锟接菜碉拷锟斤拷
    submenu1.children = &submenu1_1;
    submenu2.children = &submenu2_1;
    submenu3.children = &submenu3_1;
    submenu4.children = &submenu4_1;
    submenu5.children = &submenu5_1;
    // 锟接菜碉拷锟斤拷指锟津父硷拷锟斤拷锟剿碉拷
    submenu1_1.parent = &submenu1;
    submenu2_1.parent = &submenu2;
    submenu3_1.parent = &submenu3;
    submenu4_1.parent = &submenu4;
    submenu5_1.parent = &submenu5;
    mainmenu = submenu1;//锟斤拷锟斤拷说锟较低筹拷某锟绞硷拷锟节ｏ拷锟斤拷锟斤拷锟斤拷锟斤拷默锟斤拷锟斤拷示锟斤拷一锟斤拷锟斤拷锟剿碉拷锟斤拷
}
/*
 * 锟剿碉拷锟斤拷0页
 * 锟斤拷锟斤拷锟斤拷锟剿碉拷
 */
void Menu_Page0(void){
    ips200_show_string(Row_5, Line_0, "Menu");
    ips200_show_string(Row_0, Line_1, " ");
    ips200_show_uint(Row_11, Line_0, gnss.satellite_used,2);
    ips200_show_string(Row_1, Line_1, "Data:");
    ips200_show_string(Row_0, Line_2, " ");
    ips200_show_string(Row_1, Line_2, "Kemu1");
    ips200_show_string(Row_0, Line_3, " ");
    ips200_show_string(Row_1, Line_3, "Kemu2");
    ips200_show_string(Row_0, Line_4, " ");
    ips200_show_string(Row_1, Line_4, "Kemu3");
    ips200_show_string(Row_0, Line_5, " ");
    ips200_show_string(Row_1, Line_5, "Kemu4");
//    ips200_show_float(Row_1, Line_6, gnss.latitude, 2,6);
//    ips200_show_float(Row_1, Line_7, gnss.longitude, 2,6);
//    ips200_show_uint(Row_4, Line_6, gnss.time.hour, 2);
//    ips200_show_uint(Row_4, Line_7, gnss.time.minute, 2);
//    ips200_show_uint(Row_4, Line_8, gnss.time.second, 2);
    ips200_show_string(Row_0, 16*mainmenu.set, ">");
}
/*
 * 锟剿碉拷锟斤拷1页
 * Data(锟斤拷前锟斤拷锟斤拷锟街凤拷锟斤拷锟斤拷锟斤拷锟斤拷锟�)
 */
void Menu_Page1(void){
    ips200_show_string(Row_5, Line_0, "Data");
    ips200_show_string(Row_1, Line_1, "yaw:");
//    ips200_show_float(Row_5, Line_1, YawAngle_Trans,4,3);
    ips200_show_float(Row_1, Line_2, flow_x,3,2);
    ips200_show_float(Row_7, Line_2, flow_y,3,2);
    ips200_show_string(Row_1, Line_3, "latitude:");
    ips200_show_uint(Row_1, Line_4, gnss.latitude_deg, 3);
    ips200_show_uint(Row_1, Line_5, gnss.latitude_min, 8);
    ips200_show_string(Row_1, Line_7, "longitude:");
    ips200_show_uint(Row_1, Line_8, gnss.longitude_deg, 3);
    ips200_show_uint(Row_1, Line_9, gnss.longitude_min, 8);
    ips200_show_float(Row_1, Line_10,gnss.speed ,2,2);
    ips200_show_string(Row_1, Line_11, "pitch:");
//    ips200_show_float(Row_7, Line_11, Pitch_Angle,3,3);
    ips200_show_string(Row_1, Line_12, "roll:");
//    ips200_show_float(Row_6, Line_12, Roll_Angle,3,3);
    ips200_show_float(Row_1, Line_13, diff_x,3,2);
    ips200_show_float(Row_7, Line_13, diff_y,3,2);
    ips200_show_string(Row_1, Line_14, "direct");
    ips200_show_float(Row_7, Line_14, gnss.direction,3,2);

}
/*
 * 锟剿碉拷锟斤拷2页
 * Kemu1(锟斤拷前锟斤拷锟斤拷锟街凤拷锟斤拷锟斤拷锟斤拷锟斤拷锟�)
 */
void Menu_Page2(void){
    ips200_show_string(Row_5, Line_0, "Kemu1");
    ips200_show_string(Row_1, Line_1, "point_la_lo");
    ips200_show_string(Row_1, Line_2, "Soon_point");
    ips200_show_string(Row_1, Line_3, "point_dis");
    ips200_show_string(Row_1, Line_4, "realtime_xy");
    ips200_show_string(Row_1, Line_5, "point_xy");
    ips200_show_string(Row_1, Line_6, "Goal_speed");
}
/*
 * 锟剿碉拷锟斤拷3页
 * Kemu2(锟斤拷前锟斤拷锟斤拷锟街凤拷锟斤拷锟斤拷锟斤拷锟斤拷锟�)
 */
void Menu_Page3(void){
    ips200_show_string(Row_5, Line_0, "Kemu2");
    ips200_show_string(Row_1, Line_1, "point_la_lo");
    ips200_show_string(Row_1, Line_2, "Soon_point");
    ips200_show_string(Row_1, Line_3, "point_dis");
    ips200_show_string(Row_1, Line_4, "realtime_xy");
    ips200_show_string(Row_1, Line_5, "point_xy");
    ips200_show_string(Row_1, Line_6, "Goal_speed");
}
/*
 * 锟剿碉拷锟斤拷4页
 * Kemu3(锟斤拷前锟斤拷锟斤拷锟街凤拷锟斤拷锟斤拷锟斤拷锟斤拷锟�)
 */
void Menu_Page4(void){
    ips200_show_string(Row_5, Line_0, "Kemu3");
    ips200_show_string(Row_1, Line_1, "point_la_lo");
    ips200_show_string(Row_1, Line_2, "Soon_point");
    ips200_show_string(Row_1, Line_3, "point_dis");
    ips200_show_string(Row_1, Line_4, "realtime_xy");
    ips200_show_string(Row_1, Line_5, "point_xy");
    ips200_show_string(Row_1, Line_6, "Goal_speed");
}
/*
 * 锟剿碉拷锟斤拷5页
 * Kemu4(锟斤拷前锟斤拷锟斤拷锟街凤拷锟斤拷锟斤拷锟斤拷锟斤拷锟�)
 */
void Menu_Page5(void){
    ips200_show_string(Row_5, Line_0, "Kemu4");
    ips200_show_string(Row_1, Line_1, "point_la_lo");
    ips200_show_string(Row_1, Line_2, "Soon_point");
    ips200_show_string(Row_1, Line_3, "point_dis");
    ips200_show_string(Row_1, Line_4, "realtime_xy");
    ips200_show_string(Row_1, Line_5, "point_xy");
    ips200_show_string(Row_1, Line_6, "Goal_speed");
}
//===================================================锟解部锟斤拷锟斤拷===================================================
/*
 * 锟剿碉拷锟斤拷始锟斤拷
 */
void Menu_Init(void){
    ips200_set_dir(IPS200_PORTAIT);
    ips200_set_color(RGB565_BLACK, RGB565_WHITE);
    ips200_init(IPS200_TYPE_SPI);
    link_menu();
}
/*
 * 锟斤拷锟斤拷锟斤拷锟斤拷一锟斤拷锟剿碉拷锟斤拷
 */
void Menu_NavigateUp(void)
{
    float yaw_t =0;
    if (mainmenu.set <= 5 && mainmenu.up != NULL)//锟斤拷锟斤拷
    {
        mainmenu = *mainmenu.up;
    }
    if (mainmenu.set == 6){ //校准锟斤拷锟斤拷锟�
        yaw_t = calculateDistance_angle(0, 0, diff_x, diff_y);
//        yaw_offset = calculateOffset(Yaw_Angle,yaw_t);
//        write_float(yaw_t,0);
    }
}
/*
 * 锟斤拷锟斤拷锟斤拷锟斤拷一锟斤拷锟剿碉拷锟斤拷
 */
void Menu_NavigateDown(void)
{
    if (mainmenu.set <= 5 && mainmenu.down != NULL)
    {
        mainmenu = *mainmenu.down;
    }

}

/*
 * 锟斤拷锟斤拷锟斤拷锟接菜碉拷
 */
void Menu_Select(void)
{
    if (mainmenu.set <= 5 && mainmenu.children != NULL)
    {
        ips200_clear();
        mainmenu = *mainmenu.children;
    }
    if (mainmenu.set == 7){
        qidong =1;
        Buzzer_Beep_Once();
    }
}
/*
 * 锟斤拷锟斤拷锟斤拷母锟剿碉拷;;校准
 */
void Menu_Back(void)
{
    if (mainmenu.parent != NULL)
    {
        ips200_clear();
        mainmenu = *mainmenu.parent;

    }
    if (mainmenu.set == 1){
        //原锟斤拷
        jin_deg = gnss.longitude_deg;
        jin_min = gnss.longitude_min;
        wei_deg = gnss.latitude_deg;
        wei_min = gnss.latitude_min;
    }
}
/*
 * 锟斤拷示锟剿碉拷
 */
void Menu_show(void){
    mainmenu.function(); // 锟斤拷幕锟剿碉拷
}
