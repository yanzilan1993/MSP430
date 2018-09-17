#ifndef __UART1_H__
#define __UART1_H__

#include "data_type.h"

// USART Receiver buffer
#define RX_BUFFER_SIZE   128
#define TX_BUFFER_SIZE   128
extern unsigned char rx_buffer[RX_BUFFER_SIZE];
extern unsigned char tx_buffer[TX_BUFFER_SIZE];

typedef struct 
{
    uint16 volatile Wd_Indx;
    uint16 volatile Rd_Indx;
    uint16 Mask;
    uint8 *pbuf;
}UartBuf;

extern UartBuf UartTxbuf;//环形发送结构体
extern UartBuf UartRxbuf;//环形接收结构体

extern uint8 UART1_ISR_Flag;

extern void (*UART1_RXHandle)(uint8 *ptr);

extern uint8 UART1_RX_Buff[400];

void UartBufClear(UartBuf *Ringbuf);

void UART1NVIC_Configuration(void);
void UART1_init(uint32 pclk2,uint32 bound);
void UART1_Put_Char(unsigned char DataToSend);
uint8 Uart1_Put_Char(unsigned char DataToSend);

extern UartBuf UartTxbuf;
extern UartBuf UartRxbuf;
extern uint8 UartBuf_RD(UartBuf *Ringbuf);
extern uint16 UartBuf_Cnt(UartBuf *Ringbuf);
extern void UartBuf_WD(UartBuf *Ringbuf,uint8 DataIn);
void UartSendBuffer(uint8 *dat, uint8 len);

#endif  // __UART1_H__


