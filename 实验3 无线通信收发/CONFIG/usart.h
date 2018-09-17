#ifndef __USART_H__
#define __USART_H__

/*
    MSP430F5529 has 2 Usart:
    
    UCA0_USART:
        Tx: Pin_40  UCA0TXD     P3.3
        Rx: Pin_41  UCA0RXD     P3.4

    UCA1_USART:
        Tx: Pin_51  UCA1TXD     P4.4
        Rx: Pin_52  UCA1RXD     P4.5

    In this porgram:
        UCA0_USART Called U1
        UCA1_USART Called U2
*/

#include "usci_a_uart.h"
#include "data_type.h"
#include "stdio.h"
#include "string.h"

typedef struct
{
    uint16 tx_idle;     // 是否空闲(0: 指定的数据包还没有发送完毕)
    uint16 tx_totle;    // 发送总量
    uint16 tx_cnt;      // 已发送量
    
    uint8 tx_buf[200];  // 发送缓存    
    uint8 rx_buf[200];  // 接收缓存
}Usart_t;

extern Usart_t Usart1;
extern Usart_t Usart2;

extern void U1_UCA0_Init(uint8 ClockSource, uint32 Baudrate);
extern void U1_UCA0_Send(uint8* pTx, uint8 len);

extern void U2_UCA1_Init(uint8 ClockSource, uint32 Baudrate);
extern void U2_UCA1_Send(uint8* pTx, uint8 len);

#endif  // __USART_H__

