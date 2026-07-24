/*
 * vofa.c
 *
 *  Created on: 2025年11月29日
 *      Author: 19740
 */
//===================================================Include===================================================
#include "vofa.h"
#include "math.h"
#include "zf_driver_delay.h"
#include "zf_driver_uart.h"
#include "zf_common_fifo.h"
//===================================================定义===================================================
#define UART_INDEX            (UART_2)
#define UART_TX_PIN           (UART2_TX_P10_5)
#define UART_RX_PIN           (UART2_RX_P10_6)
#define UART_BAUDRATE         (115200)
//===================================================变量===================================================
uint8 uart_get_data[64]; //串口接受数据缓冲区
uint8 fifo_get_data[64]; //fifo输出读出缓冲区
uint8 get_data = 0; //接受数据变量
uint32 fifo_data_count = 0; //fifo数据个数
fifo_struct uart_data_fifo;

PIDNUM NUM;
//===================================================内部调用===================================================
/*
 * 将vofa中回传的数据解析
 * 使用示例 data = Get_Data();
 */
float Get_Data(void){
    uint8_t data_Start_Num = 0 ; //记录数据位开始的位置
    uint8_t data_End_Num = 0 ; //记录数据位结束的位置
//    uint8_t data_Num = 0 ; //记录数据位数
    uint8_t minux_Flag = 0 ; //判断是不是负数
    float data_return  = 0 ; //解析得到的数据
    uint8_t valid = 0;
    for (uint8_t i = 0 ; i<64 ; i++){
        if(fifo_get_data[i] == '='){
            data_Start_Num = i + 1 ;
            valid |= 0x01; //标记找到等号
        }
        if(fifo_get_data[i] == '!'){
            data_End_Num = i - 1 ;
            valid |= 0x02; //标记找到感叹号
            break;
        }
    }
    if(valid != 0x03) return 0.0f; //检查格式有效性

    //优化2：安全处理负数
    uint8_t cursor = data_Start_Num;
    if(cursor <= data_End_Num && fifo_get_data[cursor] == '-'){
        minux_Flag = 1;
        cursor ++;
    }

    //优化3：动态计算数值
    uint8_t decimal_pos = 0;
    uint8_t has_decimal = 0;
    data_return = 0.0f;

    for(uint8_t i = cursor; i <= data_End_Num; i++){
        if (fifo_get_data[i] == '.'){
            has_decimal = 1;
            decimal_pos = i - cursor;
            continue;
        }
        //字符转数字
        uint8_t digit = fifo_get_data[i] - '0'; //替换48为'0'
        if (digit > 9) return 0.0f;
        if (has_decimal)
            data_return += digit * powf(0.1, i - cursor - decimal_pos);
        else
            data_return = data_return * 10.0f + digit;
    }
    return minux_Flag ? -data_return : data_return;
}

//===================================================外部调用===================================================
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     初始化UART串口及配绕FIFO缓冲区
// 参数说明     void
// 返回参数     void
// 使用示例     Init_uart();
//-------------------------------------------------------------------------------------------------------------------
void Init_uart(void){
    fifo_init(&uart_data_fifo, FIFO_DATA_8BIT,uart_get_data,64); //初始化fifo挂载缓存
    uart_init(UART_INDEX, UART_BAUDRATE, UART_TX_PIN,UART_RX_PIN); //初始化串口
    uart_rx_interrupt(UART_INDEX, 1);

    //输出测试信息
    uart_write_string(UART_INDEX , "UART Text.");
    uart_write_byte(UART_INDEX, '\r');
    uart_write_byte(UART_INDEX, '\n');
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     验证fifo是否有数据,并发送验证结果
// 参数说明     void
// 返回参数     void
// 使用示例     Send_text();
//-------------------------------------------------------------------------------------------------------------------
void Send_text(void){
    fifo_data_count  = fifo_used(&uart_data_fifo); //查看fifo是否有数据
    if (fifo_data_count != 0){
        fifo_read_buffer(&uart_data_fifo, fifo_get_data, &fifo_data_count , FIFO_READ_AND_CLEAN);
        uart_write_string(UART_INDEX , "\r\nUART get data:");
        uart_write_buffer(UART_INDEX, fifo_get_data, fifo_data_count);
    }
    system_delay_ms(10);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     将vofa中回传的数据解析分别赋值***
// 参数说明     void
// 返回参数     void
// 使用示例     USART_PID_Adjust();
//-------------------------------------------------------------------------------------------------------------------
void USART_PID_Adjust(void){
    float data_Get = Get_Data(); //存放接收到的数据
    printf("data=%.2f\r\n", data_Get);
    if (fifo_get_data[0] == 'P'){
        if (fifo_get_data[1] == '1')
            NUM.angle_p = data_Get; //角度环P
        else if (fifo_get_data[1] == '2')
            NUM.anglev_p = data_Get; //角速度环P
        else if (fifo_get_data[1] == '3')
            NUM.speed_p = data_Get; //速度环P
    }
    else if (fifo_get_data[0] == 'I'){
        if (fifo_get_data[1] == '1')
            NUM.angle_i = data_Get; //角度环I
        else if (fifo_get_data[1] == '2')
            NUM.anglev_i = data_Get; //角速度环I
        else if (fifo_get_data[1] == '3')
            NUM.speed_i = data_Get; //速度环I
    }
    else if (fifo_get_data[0] == 'D'){
        if (fifo_get_data[1] == '1')
            NUM.angle_d = data_Get; //角度环D
        else if (fifo_get_data[1] == '2')
            NUM.anglev_d = data_Get; //角速度环D
        else if (fifo_get_data[1] == '3')
            NUM.speed_d = data_Get; //速度环D
    }
//    else if (fifo_get_data[0] == 'S'){
//        if (fifo_get_data[1] == '1')
//            Kp_base = data_Get; //舵机P
//        else if (fifo_get_data[1] == '2')
//            servo_PID.Kd = data_Get; //舵机D
//        else if (fifo_get_data[1] == '3')
//            ser_k = data_Get; //舵机I
//    }
    //电机
//    else if (fifo_get_data[0] == 'M'){
//        if (fifo_get_data[1] == '1')
//            motor_pid.Kp = data_Get; //P
//        else if (fifo_get_data[1] == '2')
//            motor_pid.Ki = data_Get; //I
//        else if (fifo_get_data[1] == '3')
//            motor_pid.Kd = data_Get; //D
//    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     UART_INDEX 的接受中断处理函数,放在UART_INDEX对应的中断调用
// 参数说明     void
// 返回参数     void
// 使用示例     uart_rx_interrupt_handler();
//-------------------------------------------------------------------------------------------------------------------
void uart_rx_interrupt_handler(void){
    uart_query_byte(UART_INDEX,&get_data);  //查询式接受数据
    fifo_write_buffer(&uart_data_fifo,&get_data,1); //将数据写入uart_data_fifo中
}
