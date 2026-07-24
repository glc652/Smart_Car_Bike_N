/*
 * flash.c
 *
 *  Created on: 2026年2月
 *      Author: 19740
 */
//===================================================Include===================================================
#include "zf_driver_flash.h"
#include "flash.h"
#include "zf_device_ips200.h"
#include "stdio.h"
//===================================================Define&Declare===================================================
#define FLASH_SECTION_INDEX       (0)  // 存储区域使用的扇区
//===================================================内部调用===================================================
//===================================================外部调用===================================================
void write_float(float data,uint32 page_num){
    //1.缓冲区清空
    flash_buffer_clear(); // 确保缓冲区已清除
    //2.将数据存到缓冲区
    flash_union_buffer[0].float_type = data;//将数据放到联合数组第一位
    //3.先判断FLASH 中有没有数据，有就把 FLASH 清除
    if(flash_check(FLASH_SECTION_INDEX, page_num))// 判断是否有数据
    {
        flash_erase_page(FLASH_SECTION_INDEX, page_num);// 清除这一页
    }
    //4.将缓冲区数据存到 FLASH
    flash_write_page_from_buffer(FLASH_SECTION_INDEX, page_num);// 向指定Flash 区域页写入缓冲区内容
}

void read_float(float *data,uint32 page_num){
//1.判断 FLASH 中有没有数据，有就将 FLASH 数据重新加载缓冲区
   if(flash_check(FLASH_SECTION_INDEX, page_num))// 判断是否有数据
   {
       flash_buffer_clear();
       flash_read_page_to_buffer(FLASH_SECTION_INDEX, page_num);// 将数据从 flash 读取到缓冲区
       *data = flash_union_buffer[0].float_type;//用什么类型写入缓冲区就用什么类型读取，然后打印或使用
   }
}
float read_point_yaw(void){
    if(flash_check(FLASH_SECTION_INDEX, 0))// 判断是否有数据
    {
       flash_buffer_clear();
       flash_read_page_to_buffer(FLASH_SECTION_INDEX, 0);
       return flash_union_buffer[0].float_type;
    }
    else{
        return 0;
    }
}
/*
 * 清除全部片上数据
 */
void clear_flash_data(void){
    int i;
    for (i=0;i<=10;i++){
        flash_erase_page(FLASH_SECTION_INDEX,i);
    }
}

//打印某一页 Flash 内容
void Flash_Debug_Print_Page(uint32 page_num, uint16 start_index, uint16 count)
{
    uint16 i;
    uint16 end_index;

    if(page_num >= EEPROM_PAGE_NUM)
    {
        printf("[FLASH] page %lu out of range, max page is %lu\r\n",
               (unsigned long)page_num, (unsigned long)(EEPROM_PAGE_NUM - 1));
        return;
    }

    if(start_index >= EEPROM_PAGE_LENGTH)
    {
        printf("[FLASH] start index %u out of range, max index is %u\r\n",
               start_index, (uint16)(EEPROM_PAGE_LENGTH - 1));
        return;
    }

    if(count == 0)
    {
        return;
    }

    end_index = start_index + count;
    if(end_index > EEPROM_PAGE_LENGTH || end_index < start_index)
    {
        end_index = EEPROM_PAGE_LENGTH;
    }

    printf("\r\n========== FLASH PAGE %lu ==========\r\n", (unsigned long)page_num);
    printf("status: %s\r\n", flash_check(FLASH_SECTION_INDEX, page_num) ? "has data" : "empty");
    printf("index              int32    float\r\n");

    flash_buffer_clear();
    flash_read_page_to_buffer(FLASH_SECTION_INDEX, page_num);

    for(i = start_index; i < end_index; i++)
    {
        printf("[%04u]    %11ld    %f\r\n",i, (long)flash_union_buffer[i].int32_type,(float)flash_union_buffer[i].int32_type/100);
    }

    printf("======== END FLASH PAGE %lu ========\r\n", (unsigned long)page_num);
}
//从起始页到结束页一页一页打印。
void Flash_Debug_Print_Pages(uint32 start_page, uint32 end_page, uint16 start_index, uint16 count)
{
    uint32 page;

    if(start_page >= EEPROM_PAGE_NUM)
    {
        printf("[FLASH] start page %lu out of range, max page is %lu\r\n",
               (unsigned long)start_page, (unsigned long)(EEPROM_PAGE_NUM - 1));
        return;
    }

    if(end_page >= EEPROM_PAGE_NUM)
    {
        end_page = EEPROM_PAGE_NUM - 1;
    }

    if(end_page < start_page)
    {
        printf("[FLASH] end page %lu is smaller than start page %lu\r\n",
               (unsigned long)end_page, (unsigned long)start_page);
        return;
    }

    for(page = start_page; page <= end_page; page++)
    {
        Flash_Debug_Print_Page(page, start_index, count);
    }
}
//可在 IPS200 上简单看某页内容
void Flash_Debug_Show_Page(uint32 page_num, uint16 start_index, uint8 lines)
{
    uint8 line;
    uint16 index;
    char text[32];

    if(page_num >= EEPROM_PAGE_NUM || start_index >= EEPROM_PAGE_LENGTH)
    {
        return;
    }

    flash_buffer_clear();
    flash_read_page_to_buffer(FLASH_SECTION_INDEX, page_num);

    ips200_clear();
    sprintf(text, "FLASH PAGE %lu", (unsigned long)page_num);
    ips200_show_string(0, 0, text);

    for(line = 0; line < lines; line++)
    {
        index = start_index + line;
        if(index >= EEPROM_PAGE_LENGTH)
        {
            break;
        }

        sprintf(text, "%04u:%08lX", index, (unsigned long)flash_union_buffer[index].uint32_type);
        ips200_show_string(0, (uint16)((line + 1) * 16), text);
    }
}

