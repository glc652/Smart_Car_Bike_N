
/*
 * Ins.h
 *
 *  Created on: 2026年5月5日
 *      Author: ORRN
 */

#ifndef CODE_INS_H_
#define CODE_INS_H_


#define MaxSize 1020          //一页存储的字节

//参数范围 <0 - 11>
#define Nag_End_Page 2        //flash中止页面
#define Nag_Start_Page 10     //flah起始页面

#define Nag_Set_mileage 440   //里程计(车轮周长)
#define Nag_Prev 200          //前瞻
#define Nag_Yaw (Gyro_YawAngle_Get()) //陀螺仪读取出来的偏航角

#define L_Mileage Mf.Left_encoder   //左轮编码器
#define R_Mileage Mf.Right_encoder//右轮编码器

#define Mileage Distance


typedef struct{
       float Final_Out; //最终输出
       float Mileage_All;   //里程计数
       float Angle_Run; //读取的偏航角
       bool Nag_Stop_f; //惯导中止flag
       uint8 Flash_read_f;//惯导读取flag
       uint16 size; //惯导数组索引通用计数
       uint16 Save_count;
       uint16 Save_index;//保存的flag
       uint8 End_f;//中止flag
       //与flash相关的
       uint8 Flash_page_index;//flash页面索引
       uint8 Flash_Save_Page_Index;//flash保存页码索引
       uint8 Nag_SystemRun_Index;   //惯导执行索引
       //暂时未开发部分
       int Prev_mile[Nag_Prev]; //前瞻
}Nagn;


extern Nagn N;   //整个变量的结构体，方便开发和移植
void Init_Nag(void);
void Nag_Run(void);
void Nag_System(void);
void Nag_Flash_Task(void);
uint8 Nag_Load_Flash_To_Array(void);
void Nag_Print_Replay_Array(uint16 start, uint16 count);
#endif /* CODE_INS_H_ */
