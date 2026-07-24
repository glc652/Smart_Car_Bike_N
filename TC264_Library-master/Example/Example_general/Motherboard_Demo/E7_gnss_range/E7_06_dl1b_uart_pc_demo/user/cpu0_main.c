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
* 开发环境          ADS v1.9.4
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
// 使用逐飞科技 英飞凌TriCore 调试下载器连接
//      直接将下载器正确连接在核心板的调试下载接口即可
//
// 使用 USB-TTL 模块连接
//      模块管脚            单片机管脚
//      USB-TTL-RX          查看 zf_common_debug.h 文件中 DEBUG_UART_TX_PIN 宏定义的引脚 默认 B6
//      USB-TTL-TX          查看 zf_common_debug.h 文件中 DEBUG_UART_RX_PIN 宏定义的引脚 默认 B7
//      USB-TTL-GND         核心板电源地 GND
//      USB-TTL-3V3         核心板 3V3 电源
//
// 接入 DL1B 模块
//      模块管脚            单片机管脚
//      SCL                 查看 zf_device_dl1b.h 中 DL1B_SCL_PIN  宏定义 默认 B8
//      SDA                 查看 zf_device_dl1b.h 中 DL1B_SDA_PIN  宏定义 默认 B9
//      XS                  查看 zf_device_dl1b.h 中 DL1B_XS_PIN  宏定义 默认 B0
//      VCC                 5V 电源
//      GND                 电源地


// *************************** 例程测试说明 ***************************
// 1.核心板烧录完成本例程
//
// 2.在断电情况下，单独使用核心板与调试下载器或者 USB-TTL 模块连接，并连接好 DL1B 模块
//      ( 如果使用配套的学习主板测试，则直接将 DL1B 模块接到对应的接口位置 )
//
// 3.使用 Type-C 给核心板供电，将调试下载器或者 USB-TTL 模块连接到电脑
//      ( 如果使用配套的学习主板测试，则需要直接使用电池供电，2S锂电池/蓝电池电压不应低于7.8V，3S锂电池电压不应低于11.3V )
//
// 4.可以在逐飞助手上位机上看到如下串口信息：
//      DL1B distance data: ...
//      DL1B distance data: ...
//
// 如果发现现象与说明严重不符 请参照本文件最下方 例程常见问题说明 进行排查


// **************************** 代码区域 ****************************
#define LED1                        (P20_9 )

#define PIT                         (CCU60_CH0 )                                 // 使用的周期中断编号 如果修改 需要同步对应修改周期中断编号与 isr.c 中的调用

uint8  pit_ms_flag          = 0;
uint16 pit_ms_count         = 0;
uint16 dl1b_refresh_freq    = 0;
int core0_main(void)
{
    clock_init();                   // 获取时钟频率<务必保留>
    debug_init();                   // 初始化默认调试串口
    // 此处编写用户代码 例如外设初始化代码等

    gpio_init(LED1, GPO, GPIO_HIGH, GPO_PUSH_PULL);                             // 初始化 LED1 输出 默认高电平 推挽输出模式

    while(1)
    {
        if(dl1b_init())
    {
    printf("\r\nDL1B init error.");                                     // DL1B 初始化失败
    }
    else
    {
        break;
    }
        gpio_toggle_level(LED1);                                                // 翻转 LED 引脚输出电平 控制 LED 亮灭 初始化出错这个灯会闪的很慢
    }
    pit_ms_init(PIT, 5);

    // 此处编写用户代码 例如外设初始化代码等
    cpu_wait_event_ready();         // 等待所有核心初始化完毕
    while (TRUE)
    {
        // 此处编写需要循环执行的代码

        if(pit_ms_flag)
        {
            printf("\r\nDL1B distance data: %5d", dl1b_distance_mm);
            printf("\r\nDL1B refresh freq: %5d", dl1b_refresh_freq);
            gpio_toggle_level(LED1);                                            // 翻转 LED 引脚输出电平 控制 LED 亮灭
            dl1b_refresh_freq = 0;
            pit_ms_flag = 0;
        }

        // 此处编写需要循环执行的代码
    }
}

IFX_INTERRUPT(cc60_pit_ch0_isr, 0, CCU6_0_CH0_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    pit_clear_flag(CCU60_CH0);

#if (!DL1B_INT_ENABLE)
    if(0 == pit_ms_count % 10)                                                  // 每 10ms 获取一次测距信息 周期 10 ms 频率 100Hz
    {
        dl1b_get_distance();                                                    // 测距调用频率不应高于 100Hz 周期不应低于 10ms
    }
#endif
    // 通过判断 dl1a_finsh_flag 来确定是否更新了数据 记得获取了数据后就清空标志位
    if(dl1b_finsh_flag)
    {
        dl1b_refresh_freq ++;
        dl1b_finsh_flag = 0;
    }
    pit_ms_count = (pit_ms_count == 995) ? (0) : (pit_ms_count + 5);            // 1000ms 周期计数
    if(0 == pit_ms_count % 1000)
    {
        pit_ms_flag = 1;
    }

}

#pragma section all restore
// **************************** 代码区域 ****************************

// *************************** 例程常见问题说明 ***************************
// 遇到问题时请按照以下问题检查列表检查
//
// 问题1：串口没有数据
//      查看逐飞助手上位机打开的是否是正确的串口，检查打开的 COM 口是否对应的是调试下载器或者 USB-TTL 模块的 COM 口
//      如果是使用逐飞科技 英飞凌TriCore 调试下载器连接，那么检查下载器线是否松动，检查核心板串口跳线是否已经焊接，串口跳线查看核心板原理图即可找到
//      如果是使用 USB-TTL 模块连接，那么检查连线是否正常是否松动，模块 TX 是否连接的核心板的 RX，模块 RX 是否连接的核心板的 TX
//
// 问题2：串口数据乱码
//      查看逐飞助手上位机设置的波特率是否与程序设置一致，程序中 zf_common_debug.h 文件中 DEBUG_UART_BAUDRATE 宏定义为 debug uart 使用的串口波特率
//
// 问题3：串口输出 DL1B init error.
//      检查 DL1B 的接线是否正确是否松动
//      检查 DL1B 的模块是不是坏了
//      给信号线加上拉看看
//
// 问题4：DL1B 数值异常 显示 819x 或者一个固定数据
//      测距有效范围为 1400mm 超过这个距离会显示 819x 的无效值
//      如果数据显示正常，突然变为一个固定值不再变化，那么尝试断电重启，如果重启后正常数据，证明是 XS 信号上受到干扰导致模块丢失数据
