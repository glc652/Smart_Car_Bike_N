/*
 * Ins.c
 *
 *  Created on: 2026年5月5日
 *      Author: ORRN
 */

#include "zf_common_headfile.h"
#include "Ins.h"
Nagn N;
#define NAG_REPLAY_MAX_POINTS   ((Nag_Start_Page - Nag_End_Page) * MaxSize)
static int32 nag_replay_buf[NAG_REPLAY_MAX_POINTS];
static uint16 nag_replay_total = 0;
static uint16 nag_replay_index = 0;

/*
 * 将当前 Flash 缓冲区写入 `N.Flash_page_index` 页
 */
void flash_Nag_Write(){

    // 调试：打印当前准备写入数据页的前5条数据
    for (int i = 0; i < 5 && i < N.size; i++) {
        printf("Before write: buffer[%d] = %d (angle=%.2f)\n",
               i, flash_union_buffer[i].int32_type,
               flash_union_buffer[i].int32_type / 100.0f);
    }
    printf("N.size=%d, N.Save_index=%d\n", N.size, N.Save_index);

    if(flash_check(0, N.Flash_page_index))flash_erase_page(0, N.Flash_page_index);
    flash_write_page_from_buffer(0,N.Flash_page_index);

    if(N.End_f == 1)
    {
     flash_buffer_clear();
     flash_union_buffer[0].uint32_type = N.Save_index;
     flash_union_buffer[1].uint32_type = ~((uint32)N.Save_index);
     if(flash_check(0, Nag_End_Page))flash_erase_page(0, Nag_End_Page);
     flash_write_page_from_buffer(0,Nag_End_Page);
    }

    flash_buffer_clear();

}

/*
 * 读取 Flash 中保存的数据。
 */
void flash_Nag_Read(){
    flash_buffer_clear();
    static uint8 Index_R_f=0;

    if( 0 == Index_R_f)
    {
       flash_read_page_to_buffer(0,Nag_End_Page);
        N.Save_index = flash_union_buffer[0].uint32_type;
        Index_R_f=1;
        flash_buffer_clear();
    }
    if(flash_check(0, N.Flash_page_index))
    {
        flash_read_page_to_buffer(0, N.Flash_page_index);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     偏航角存入
// 参数说明     将读取的YAW存储到flash中存储
// 返回参数     void
// 使用示例     用户无需调用
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8 nag_flash_write_req = 0;
uint8 nag_flash_write_busy = 0;
static int32 nag_write_buf[MaxSize];
static uint16 nag_write_count = 0;
void Run_Nag_Save(void)
{
    N.Mileage_All += Encoder_Nag_Get_Delta_Pulse();
//     printf("Mileage_All=%f,A_SPEED=%f\r\n", N.Mileage_All,A_SPEED);
    if(N.size > MaxSize)//当大于这页有的flash大小的时候，写入一次，防止重复写入
    {
        nag_flash_write_req = 1;
        return;
    }
    if(N.Mileage_All >= Nag_Set_mileage)    //大于你的设定值的时候
    {
        int32 Save=(int32)(ps2_yaw_target*100); //读取的偏航角放大100倍，避免使用Float类型来存储Nag_Yaw
        flash_union_buffer[N.size++].int32_type = Save;
        N.Save_index++;
       if(N.Mileage_All > 0) N.Mileage_All -= Nag_Set_mileage;//重置历程计数字//保存到flash
//       else N.Mileage_All += Nag_Set_mileage;//倒车
    }

}

void Nag_Flash_Task(void)
{
    if(!nag_flash_write_req) return;
    nag_flash_write_req = 0;
    flash_Nag_Write();
    N.size = 0;
    N.Flash_page_index--;
    if(N.Flash_page_index <= Nag_End_Page){
      N.End_f = 1;
    }
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     读取偏航角的线程函数
// 参数说明     读取偏航角的线程函数，通过切换N.End_f来切换线程
// 返回参数     void
// 使用示例     用户无需调用
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Nag_Read()
{
        switch(N.End_f)
        {
            case 0:Run_Nag_Save();  //默认执行函数
                break;
            case 1:
                flash_Nag_Write();  //写入最后一页，保证flash存储满
                N.size = 0;
                N.Nag_SystemRun_Index = 0;
                N.End_f++;
                break;
            case 2:
                N.End_f++;  //结束线程
                break;
        }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     惯导参数初始化
// 返回参数     void
// 使用示例     放入程序执行开始
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Init_Nag(void)
{
    memset(&N, 0, sizeof(N));
    N.Flash_page_index=Nag_Start_Page;
    Encoder_Nag_Pulse_Reset();
    flash_buffer_clear();
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     偏航角复现
// 参数说明     读取flash中存储的YAW
// 返回参数     void
// 使用示例     用户无需调用
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Run_Nag_GPS()
{
    uint8 offset=0;//计数偏移
    N.Mileage_All += Encoder_Nag_Get_Delta_Pulse();
//    if(N.Flash_read_f == 0)//同样是每次翻页只读取一次，防止反复读取
//    {
//        N.size=offset;
//        flash_Nag_Read();   //flash读取
//        N.Flash_read_f=1;
//    }
//    if(N.size >= MaxSize)    //当大于设定的flsh大小的时候
//    {
//        N.Flash_read_f=0;   //读取flash重置
//        N.Flash_page_index--;   //页面减少
//        N.size = 0;
//        zf_assert(N.Flash_page_index > Nag_End_Page);
//    }
//    printf("N.Angle:%f\n",N.Angle_Run);
//   printf("N.Angle_Run=%f,N.Save_index=%d, N.Flash_page_index=%d,N.Nag_Stop_f=%d\r\n", N.Angle_Run, N.Save_index, N.Flash_page_index, N.Nag_Stop_f);
    if(N.Mileage_All >= Nag_Set_mileage)
    {
        //偏差计算
        N.Save_count++;
//        if(N.Save_count > N.Save_index-2)
//        {
//            N.Nag_Stop_f++;
//            return;
//        }
//       N.Angle_Run = (flash_union_buffer[N.size++].int32_type / 100.0f);
//       if(N.Mileage_All > 0) N.Mileage_All -= Nag_Set_mileage;//重置历程计数字//保存到flash
//       else N.Mileage_All += Nag_Set_mileage;   //倒车

       if(nag_replay_index >= nag_replay_total){
         /* keep last replay angle */
         if(N.Mileage_All > 0) N.Mileage_All -= Nag_Set_mileage;//重置历程计数字//保存到flash
         return;
        }
        N.Angle_Run = nag_replay_buf[nag_replay_index++] / 100.0f;
       if(N.Mileage_All > 0) N.Mileage_All -= Nag_Set_mileage;//重置历程计数字//保存到flash
    }


}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     用于生成偏差计算
// 参数说明     N.Final_Out为最终生成的偏差大小
// 返回参数     void
// 使用示例     用户无需调用
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Nag_Run()
{
  if(N.Nag_Stop_f){
    Servo_Free();
    Motor_Disable();

    N.Final_Out=0;

    return;
  }
  Run_Nag_GPS();  //偏航角读取复现

//    N.Final_Out=N.Angle_Run-Nag_Yaw;//负数向右转，正数向左转
//        printf("%f,%d\r\n",N.Angle_Run,N.Flash_page_index);
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     将Flash里的数据存到数组中
// 返回参数     void
// 使用示例     用户无需调用
//-------------------------------------------------------------------------------------------------------------------
uint8 Nag_Load_Flash_To_Array(void)
{
  uint8 page; //当前正在读取的 Flash 页号
  uint16 i; //当前页内索引
  uint16 count = 0; //已经加载到 RAM 数组里的点数
  flash_read_page_to_buffer(0, Nag_End_Page);
  N.Save_index = flash_union_buffer[0].uint32_type; //读取总记录点数
  if(N.Save_index == 0 || N.Save_index > NAG_REPLAY_MAX_POINTS) {
      return 0;
  }
  for(page = Nag_Start_Page; page > Nag_End_Page; page--){
      flash_read_page_to_buffer(0, page);
      for(i = 0; i < MaxSize && count < N.Save_index; i++){
          nag_replay_buf[count++] = flash_union_buffer[i].int32_type;//把当前页里的角度点复制到 RAM 数组
      }
      if(count >= N.Save_index) {
          break;
      }
  }
  nag_replay_total = count;
  nag_replay_index = 0;
  return 1;
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     惯性导航执行函数
// 参数说明     index           索引
// 参数说明     type            类型值
// 返回参数     void
// 使用示例     放入中断中
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Nag_System(void){
    static uint8 last_run_index = 0;

    if(N.Nag_SystemRun_Index != last_run_index)
    {
        Encoder_Nag_Pulse_Reset();
        N.Mileage_All = 0;
        last_run_index = N.Nag_SystemRun_Index;
    }

    //卫保护
    if(!N.Nag_SystemRun_Index || N.Nag_Stop_f )
    {
        Encoder_Nag_Pulse_Reset();
        return;
    }

    switch(N.Nag_SystemRun_Index)
    {
        case 1 : Nag_Read();    //1是读取
            break;
        case 2: Nag_Run(); //2是复现
            break;

    }
}
//测试
void Nag_Print_Replay_Array(uint16 start, uint16 count)
  {
      uint16 i;
      uint16 end;

      if(nag_replay_total == 0)
      {
          printf("NAG replay array empty\r\n");
          return;
      }

      if(start >= nag_replay_total)
      {
          printf("NAG print start out of range: start=%u total=%u\r\n",
                 start, nag_replay_total);
          return;
      }

      end = start + count;
      if(end > nag_replay_total || end < start)
      {
          end = nag_replay_total;
      }

      printf("\r\n========== NAG REPLAY ARRAY ==========\r\n");
      printf("total=%u, index=%u, print=[%u,%u)\r\n",
             nag_replay_total, nag_replay_index, start, end);
      printf("idx      raw(int32)      angle(deg)\r\n");

      for(i = start; i < end; i++)
      {
          printf("[%04u]   %11ld      %.2f\r\n",
                 i,
                 (long)nag_replay_buf[i],
                 (float)nag_replay_buf[i] / 100.0f);
      }

      printf("======== END NAG REPLAY ARRAY ========\r\n");
  }

