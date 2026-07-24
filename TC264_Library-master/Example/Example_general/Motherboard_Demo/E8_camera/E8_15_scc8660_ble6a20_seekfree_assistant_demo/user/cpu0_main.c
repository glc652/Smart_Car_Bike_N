/*********************************************************************************************************************
* TC264 Opensourec Library 即（TC264 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是 TC264 开源库的一部分
*
* TC264 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          cpu0_main
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          ADS v1.9.20
* 适用平台          TC264D
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2022-09-15       pudding            first version
********************************************************************************************************************/
#include "zf_common_headfile.h"
#pragma section all "cpu0_dsram"
// 将本语句与#pragma section all restore语句之间的全局变量都放在CPU0的RAM中

// *************************** 例程硬件连接说明 ***************************
// 使用逐飞科技 WCH-LINK 调试下载器连接
//      直接将下载器正确连接在核心板的调试下载接口即可
//
// 接入蓝牙转串口模块
//      模块管脚            单片机管脚
//      RX                  查看 zf_device_ble6a20.h 中 BLE6A20_RX_PINx  宏定义 默认 P10_6
//      TX                  查看 zf_device_ble6a20.h 中 BLE6A20_TX_PINx  宏定义 默认 P10_5
//      RTS                 查看 zf_device_ble6a20.h 中 BLE6A20_RTS_PINx 宏定义 默认 P10_2
//      GND                 核心板电源地 GND
//      3V3                 核心板 3V3 电源
//
// 接入凌瞳彩色摄像头 对应主板摄像头接口 请注意线序
//      模块管脚            单片机管脚
//      TXD                 查看 zf_device_scc8660.h 中 SCC8660_COF_UART_TX 宏定义 默认 P02_3  总钻风 UART-TX 引脚 要接在单片机 RX 上
//      RXD                 查看 zf_device_scc8660.h 中 SCC8660_COF_UART_RX 宏定义 默认 P02_2 总钻风 UART-RX 引脚 要接在单片机 TX 上
//      D0                  查看 zf_device_scc8660.h 中 SCC8660_D0_PIN      宏定义 默认 P00_0
//      D1                  查看 zf_device_scc8660.h 中 SCC8660_D1_PIN      宏定义 默认 P00_1
//      D2                  查看 zf_device_scc8660.h 中 SCC8660_D2_PIN      宏定义 默认 P00_2
//      D3                  查看 zf_device_scc8660.h 中 SCC8660_D3_PIN      宏定义 默认 P00_3
//      D4                  查看 zf_device_scc8660.h 中 SCC8660_D4_PIN      宏定义 默认 P00_4
//      D5                  查看 zf_device_scc8660.h 中 SCC8660_D5_PIN      宏定义 默认 P00_5
//      D6                  查看 zf_device_scc8660.h 中 SCC8660_D6_PIN      宏定义 默认 P00_6
//      D7                  查看 zf_device_scc8660.h 中 SCC8660_D7_PIN      宏定义 默认 P00_7
//      PCLK                查看 zf_device_scc8660.h 中 SCC8660_PCLK_PIN    宏定义 默认 P02_1
//      VSYNC               查看 zf_device_scc8660.h 中 SCC8660_VSY_PIN     宏定义 默认 P02_0


// *************************** 例程测试说明 ***************************
// 1.核心板烧录完成本例程 将核心板插在主板上 插到底 摄像头接在主板的摄像头接口 注意线序
//
// 2.先将蓝牙转串口模块USB端连接电脑,再将主板上电 或者核心板供电
//
// 3.电脑上使用摄像头的传感器调试助手 串口波特率为 zf_common_debug.h 文件中 DEBUG_UART_BAUDRATE 宏定义 默认 115200，核心板按下复位按键
//
// 4.调试助手选择彩色图像 分辨率填写宽160 高120 勾选开始采集再打开对应的串口
//
// 5.上位机将会显示图像
//
// 如果发现现象与说明严重不符 请参照本文件最下方 例程常见问题说明 进行排查


// **************************** 代码区域 ****************************
#define LED1                    (P20_9 )

//0：不包含边界信息
//1：包含三条边线信息，边线信息只包含横轴坐标，纵轴坐标由图像高度得到，意味着每个边界在一行中只会有一个点
//2：包含三条边线信息，边界信息只含有纵轴坐标，横轴坐标由图像宽度得到，意味着每个边界在一列中只会有一个点，一般来说很少有这样的使用需求
//3：包含三条边线信息，边界信息含有横纵轴坐标，意味着你可以指定每个点的横纵坐标，边线的数量也可以大于或者小于图像的高度，通常来说边线数量大于图像的高度，一般是搜线算法能找出回弯的情况
//4：没有图像信息，仅包含三条边线信息，边线信息只包含横轴坐标，纵轴坐标由图像高度得到，意味着每个边界在一行中只会有一个点，这样的方式可以极大的降低传输的数据量
#define INCLUDE_BOUNDARY_TYPE   0


// 边界的点数量远大于图像高度，便于保存回弯的情况
#define BOUNDARY_NUM            (SCC8660_H * 3 / 2)

// 只有X边界
uint8 xy_x1_boundary[BOUNDARY_NUM], xy_x2_boundary[BOUNDARY_NUM], xy_x3_boundary[BOUNDARY_NUM];

// 只有Y边界
uint8 xy_y1_boundary[BOUNDARY_NUM], xy_y2_boundary[BOUNDARY_NUM], xy_y3_boundary[BOUNDARY_NUM];

// X Y边界都是单独指定的
uint8 x1_boundary[SCC8660_H], x2_boundary[SCC8660_H], x3_boundary[SCC8660_H];
uint8 y1_boundary[SCC8660_W], y2_boundary[SCC8660_W], y3_boundary[SCC8660_W];

uint16   scc8660_image_save[SCC8660_H][SCC8660_W];      // 图像备份，防止访问冲突
#if(0 != INCLUDE_BOUNDARY_TYPE)
    int32 i = 0;
#endif
#if(3 <= INCLUDE_BOUNDARY_TYPE)
    int32 j = 0;
#endif


// **************************** 代码区域 ****************************
int core0_main(void)
{
    clock_init();                   // 获取时钟频率<务必保留>
    debug_init();                   // 初始化默认调试串口
    // 此处编写用户代码 例如外设初始化代码等

    gpio_init(LED1, GPO, GPIO_HIGH, GPO_PUSH_PULL);                             // 初始化 LED1 输出 默认高电平 推挽输出模式

    // 推荐先初始化摄像头，后初始化逐飞助手
    while(1)
    {
        if(scc8660_init())
            gpio_toggle_level(LED1);                                            // 翻转 LED 引脚输出电平 控制 LED 亮灭 初始化出错这个灯会闪的很慢
        else
            break;
        system_delay_ms(1000);                                                  // 闪灯表示异常
    }

    // 逐飞助手初始化 数据传输使用蓝牙串口
    ble6a20_init();
    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_BLE6A20);

    // 如果要发送图像信息，则务必调用seekfree_assistant_camera_information_config函数进行必要的参数设置
    // 如果需要发送边线则还需调用seekfree_assistant_camera_boundary_config函数设置边线的信息

    #if(0 == INCLUDE_BOUNDARY_TYPE)
        // 发送总钻风图像信息(仅包含原始图像信息)
        seekfree_assistant_camera_information_config(SEEKFREE_ASSISTANT_SCC8660, scc8660_image_save[0], SCC8660_W, SCC8660_H);


    #elif(1 == INCLUDE_BOUNDARY_TYPE)
        // 发送总钻风图像信息(并且包含三条边界信息，边界信息只含有横轴坐标，纵轴坐标由图像高度得到，意味着每个边界在一行中只会有一个点)
        // 对边界数组写入数据
        for(i = 0; i < SCC8660_H; i++)
        {
            x1_boundary[i] = 70 - (70 - 20) * i / SCC8660_H;
            x2_boundary[i] = SCC8660_W / 2;
            x3_boundary[i] = 90 + (140 - 90) * i / SCC8660_H;
        }
        seekfree_assistant_camera_information_config(SEEKFREE_ASSISTANT_SCC8660, scc8660_image[0], SCC8660_W, SCC8660_H);
        seekfree_assistant_camera_boundary_config(X_BOUNDARY, SCC8660_H, x1_boundary, x2_boundary, x3_boundary, NULL, NULL ,NULL);


    #elif(2 == INCLUDE_BOUNDARY_TYPE)
        // 发送总钻风图像信息(并且包含三条边界信息，边界信息只含有纵轴坐标，横轴坐标由图像宽度得到，意味着每个边界在一列中只会有一个点)
        // 通常很少有这样的使用需求
        // 对边界数组写入数据
        for(i = 0; i < SCC8660_W; i++)
        {
            y1_boundary[i] = i * SCC8660_H / SCC8660_W;
            y2_boundary[i] = SCC8660_H / 2;
            y3_boundary[i] = (SCC8660_W - i) * SCC8660_H / SCC8660_W;
        }
        seekfree_assistant_camera_information_config(SEEKFREE_ASSISTANT_SCC8660, scc8660_image[0], SCC8660_W, SCC8660_H);
        seekfree_assistant_camera_boundary_config(Y_BOUNDARY, SCC8660_W, NULL, NULL ,NULL, y1_boundary, y2_boundary, y3_boundary);


    #elif(3 == INCLUDE_BOUNDARY_TYPE)
        // 发送总钻风图像信息(并且包含三条边界信息，边界信息含有横纵轴坐标)
        // 这样的方式可以实现对于有回弯的边界显示
        j = 0;
        for(i = SCC8660_H - 1; i >= SCC8660_H / 2; i--)
        {
            // 直线部分
            xy_x1_boundary[j] = 34;
            xy_y1_boundary[j] = i;

            xy_x2_boundary[j] = 47;
            xy_y2_boundary[j] = i;

            xy_x3_boundary[j] = 60;
            xy_y3_boundary[j] = i;
            j++;
        }

        for(i = SCC8660_H / 2 - 1; i >= 0; i--)
        {
            // 直线连接弯道部分
            xy_x1_boundary[j] = 34 + (SCC8660_H / 2 - i) * (SCC8660_W / 2 - 34) / (SCC8660_H / 2);
            xy_y1_boundary[j] = i;

            xy_x2_boundary[j] = 47 + (SCC8660_H / 2 - i) * (SCC8660_W / 2 - 47) / (SCC8660_H / 2);
            xy_y2_boundary[j] = 15 + i * 3 / 4;

            xy_x3_boundary[j] = 60 + (SCC8660_H / 2 - i) * (SCC8660_W / 2 - 60) / (SCC8660_H / 2);
            xy_y3_boundary[j] = 30 + i / 2;
            j++;
        }

        for(i = 0; i < SCC8660_H / 2; i++)
        {
            // 回弯部分
            xy_x1_boundary[j] = SCC8660_W / 2 + i * (138 - SCC8660_W / 2) / (SCC8660_H / 2);
            xy_y1_boundary[j] = i;

            xy_x2_boundary[j] = SCC8660_W / 2 + i * (133 - SCC8660_W / 2) / (SCC8660_H / 2);
            xy_y2_boundary[j] = 15 + i * 3 / 4;

            xy_x3_boundary[j] = SCC8660_W / 2 + i * (128 - SCC8660_W / 2) / (SCC8660_H / 2);
            xy_y3_boundary[j] = 30 + i / 2;
            j++;
        }
        seekfree_assistant_camera_information_config(SEEKFREE_ASSISTANT_SCC8660, scc8660_image[0], SCC8660_W, SCC8660_H);
        seekfree_assistant_camera_boundary_config(XY_BOUNDARY, BOUNDARY_NUM, xy_x1_boundary, xy_x2_boundary, xy_x3_boundary, xy_y1_boundary, xy_y2_boundary, xy_y3_boundary);


    #elif(4 == INCLUDE_BOUNDARY_TYPE)
        // 发送总钻风图像信息(并且包含三条边界信息，边界信息只含有横轴坐标，纵轴坐标由图像高度得到，意味着每个边界在一行中只会有一个点)
        // 对边界数组写入数据
        for(i = 0; i < SCC8660_H; i++)
        {
            x1_boundary[i] = 70 - (70 - 20) * i / SCC8660_H;
            x2_boundary[i] = SCC8660_W / 2;
            x3_boundary[i] = 90 + (140 - 90) * i / SCC8660_H;
        }
        seekfree_assistant_camera_information_config(SEEKFREE_ASSISTANT_SCC8660, NULL, SCC8660_W, SCC8660_H);
        seekfree_assistant_camera_boundary_config(X_BOUNDARY, SCC8660_H, x1_boundary, x2_boundary, x3_boundary, NULL, NULL ,NULL);


    #endif


    // 此处编写用户代码 例如外设初始化代码等
    cpu_wait_event_ready();         // 等待所有核心初始化完毕
    while (TRUE)
    {
        // 此处编写需要循环执行的代码

        if(scc8660_finish_flag)
        {

            memcpy(scc8660_image_save[0], scc8660_image[0], SCC8660_IMAGE_SIZE);
            // 发送图像
            seekfree_assistant_camera_send();

            scc8660_finish_flag = 0;
        }


        // 此处编写需要循环执行的代码
    }
}

#pragma section all restore
// **************************** 代码区域 ****************************

// *************************** 例程常见问题说明 ***************************
// 遇到问题时请按照以下问题检查列表检查
//
// 问题1：上位机没有图像
//      查看串口助手打开的是否是正确的串口，检查打开的 COM 口是否对应的是调试下载器或者 USB-TTL 模块的 COM 口
//      如果串口和接线都正常 查看LED灯是否闪烁 如果闪烁证明初始化失败 检查摄像头接线是否正常 线序是否正确
//      如果摄像头接线都正常 LED灯不闪烁 联系技术客服即可
//      如果是使用蓝牙转串口模块，请检查蓝牙串口的波特率，地址，频率是否一致
//
// 问题2：图像乱码、错位、不正常
//      检查是否选择的灰度图像选项
//      检查是否正确设置了图像宽度和高度 宽度和高度查看 zf_device_mt9v03x.h 中 MT9V03X_W 和 MT9V03X_H 宏定义
