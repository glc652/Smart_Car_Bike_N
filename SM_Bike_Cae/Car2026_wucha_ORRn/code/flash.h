/*
 * flash.h
 *
 *  Created on: 2026年2月22日
 *      Author: 19740
 */

#ifndef CODE_FLASH_H_
#define CODE_FLASH_H_

void write_float(float data,uint32 page_num);
void read_float(float *data,uint32 page_num);
void clear_flash_data(void);
float read_point_yaw(void);
void Flash_Debug_Print_Page(uint32 page_num, uint16 start_index, uint16 count);
void Flash_Debug_Print_Pages(uint32 start_page, uint32 end_page, uint16 start_index, uint16 count);
void Flash_Debug_Show_Page(uint32 page_num, uint16 start_index, uint8 lines);
#endif /* CODE_FLASH_H_ */

