/*
 * vofa.h
 *
 *  Created on: 2025年11月29日
 *      Author: glc
 */

#ifndef CODE_VOFA_H_
#define CODE_VOFA_H_

typedef struct{
    //角度环
    float angle_p;
    float angle_i;
    float angle_d;
    //角速度环
    float anglev_p;
    float anglev_i;
    float anglev_d;
    //速度环
    float speed_p;
    float speed_i;
    float speed_d;

}PIDNUM;
extern PIDNUM NUM;
void Init_uart(void);
void Send_text(void);
void uart_rx_interrupt_handler(void);
void USART_PID_Adjust(void);

#endif /* CODE_VOFA_H_ */
