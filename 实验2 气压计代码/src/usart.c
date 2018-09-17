#include "usart.h"
#include "gpio.h"
#include "ucs.h"
#include "rc.h"

extern Usart_t Usart1 = 
{
    .tx_idle = 1,
    .tx_totle = 0,
    .tx_cnt = 0,
};

extern Usart_t Usart2 = 
{
    .tx_idle = 1,
    .tx_totle = 0,
    .tx_cnt = 0,
};

extern void U1_UCA0_Init(uint8 ClockSource, uint32 Baudrate)
{
    USCI_A_UART_initParam USCI_A0_UART_initParam;
    
    float frequency = 0.0;
    float N_f = 0.0;
    
    if(USCI_A_UART_CLOCKSOURCE_ACLK == ClockSource)
    {
        frequency = UCS_getACLK();
    }
    else if(USCI_A_UART_CLOCKSOURCE_SMCLK == ClockSource)
    {
        frequency = UCS_getSMCLK();
    }
    else
    {
      return;
    }
    
    // Tx: P3.3     Rx: P3.4
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3, GPIO_PIN3);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN4);
    
    N_f = frequency / Baudrate;
    
    USCI_A0_UART_initParam.selectClockSource = ClockSource;
    USCI_A0_UART_initParam.clockPrescalar = (uint16)(N_f / 16);    
    USCI_A0_UART_initParam.firstModReg = (uint16)(((N_f / 16) - (uint16)(N_f / 16)) * 16);
    USCI_A0_UART_initParam.secondModReg = 0;
    USCI_A0_UART_initParam.parity = USCI_A_UART_NO_PARITY;
    USCI_A0_UART_initParam.msborLsbFirst = USCI_A_UART_LSB_FIRST;
    USCI_A0_UART_initParam.numberofStopBits = USCI_A_UART_ONE_STOP_BIT;
    USCI_A0_UART_initParam.uartMode = USCI_A_UART_MODE;
    USCI_A0_UART_initParam.overSampling = USCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;
    
    USCI_A_UART_init(USCI_A0_BASE, &USCI_A0_UART_initParam);
    
    USCI_A_UART_enable(USCI_A0_BASE);
    
    /*
        http://bbs.eeworld.com.cn/forum.php?mod=viewthread&tid=430117&highlight=msp430%2B%B4%AE%BF%DA%D6%D0%B6%CF
        串口中断使能要放在串口初始化结束之后 否则不能进入中断
    */
    USCI_A_UART_enableInterrupt(USCI_A0_BASE, USCI_A_UART_TRANSMIT_INTERRUPT);
    USCI_A_UART_enableInterrupt(USCI_A0_BASE, USCI_A_UART_RECEIVE_INTERRUPT);
}

extern void U1_UCA0_Send(uint8* pTx, uint8 len)
{
    uint8 i;
    
    // 等待串口空闲
    while(!Usart1.tx_idle)
        ;
    
    // 占用串口
    Usart1.tx_idle = 0;
    
    // 缓存要发送的数据
    for(i=0; i<len; i++)
        Usart1.tx_buf[i] = pTx[i];
    
    // 启动发送
    Usart1.tx_totle = len;
    Usart1.tx_cnt = 0;
    
    if(Usart1.tx_cnt < Usart1.tx_totle)
        UCA0TXBUF = Usart1.tx_buf[Usart1.tx_cnt++];
}

extern void U2_UCA1_Init(uint8 ClockSource, uint32 Baudrate)
{    
    USCI_A_UART_initParam USCI_A1_UART_initParam;
    
    float frequency = 0.0;
    float N_f = 0.0;
    
    if(USCI_A_UART_CLOCKSOURCE_ACLK == ClockSource)
    {
        frequency = UCS_getACLK();
    }
    else if(USCI_A_UART_CLOCKSOURCE_SMCLK == ClockSource)
    {
        frequency = UCS_getSMCLK();
    }
    else
    {
      return;
    }
    
    // Tx: P4.4     Rx: P4.5
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN5);
    
    N_f = frequency / Baudrate;
    
    USCI_A1_UART_initParam.selectClockSource = ClockSource;
    USCI_A1_UART_initParam.clockPrescalar = (uint16)(N_f / 16);    
    USCI_A1_UART_initParam.firstModReg = (uint16)(((N_f / 16) - (uint16)(N_f / 16)) * 16);
    USCI_A1_UART_initParam.secondModReg = 0;
    USCI_A1_UART_initParam.parity = USCI_A_UART_NO_PARITY;
    USCI_A1_UART_initParam.msborLsbFirst = USCI_A_UART_LSB_FIRST;
    USCI_A1_UART_initParam.numberofStopBits = USCI_A_UART_ONE_STOP_BIT;
    USCI_A1_UART_initParam.uartMode = USCI_A_UART_MODE;
    USCI_A1_UART_initParam.overSampling = USCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;
    
    USCI_A_UART_init(USCI_A1_BASE, &USCI_A1_UART_initParam);
    
    USCI_A_UART_enable(USCI_A1_BASE);
    
    /*
        http://bbs.eeworld.com.cn/forum.php?mod=viewthread&tid=430117&highlight=msp430%2B%B4%AE%BF%DA%D6%D0%B6%CF
        串口中断使能要放在串口初始化结束之后 否则不能进入中断
    */
    USCI_A_UART_enableInterrupt(USCI_A1_BASE, USCI_A_UART_TRANSMIT_INTERRUPT);
    USCI_A_UART_enableInterrupt(USCI_A1_BASE, USCI_A_UART_RECEIVE_INTERRUPT);
}

extern void U2_UCA1_Send(uint8* pTx, uint8 len)
{
    uint8 i;
    
    // 等待串口空闲
    while(!Usart2.tx_idle)
        ;
    
    // 占用串口
    Usart2.tx_idle = 0;
    
    // 缓存要发送的数据
    for(i=0; i<len; i++)
        Usart2.tx_buf[i] = pTx[i];
    
    // 启动发送
    Usart2.tx_totle = len;
    Usart2.tx_cnt = 0;
    
    if(Usart2.tx_cnt < Usart2.tx_totle)
        UCA1TXBUF = Usart2.tx_buf[Usart2.tx_cnt++];
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
    #pragma vector = USCI_A0_VECTOR
    __interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
    void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
    #error Compiler not supported!
#endif
{
    // USART Tx Interrupt
    if(USCI_A_UART_getInterruptStatus(USCI_A0_BASE, USCI_A_UART_TRANSMIT_INTERRUPT_FLAG))
    {
        if(Usart1.tx_cnt < Usart1.tx_totle)
            UCA0TXBUF = Usart1.tx_buf[Usart1.tx_cnt++];
        else
            Usart1.tx_idle = 1;
        
        USCI_A_UART_clearInterrupt(USCI_A0_BASE, USCI_A_UART_TRANSMIT_INTERRUPT_FLAG);
    }
    
    // USART Rx Interrupt
    if(USCI_A_UART_getInterruptStatus(USCI_A0_BASE, USCI_A_UART_RECEIVE_INTERRUPT_FLAG))
    {
//        UCA0TXBUF = UCA0RXBUF;
        //RC_Pkg_Decode(UCA0RXBUF);
        
        USCI_A_UART_clearInterrupt(USCI_A0_BASE, USCI_A_UART_RECEIVE_INTERRUPT_FLAG);
    }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
    #pragma vector = USCI_A1_VECTOR
    __interrupt void USCI_A1_ISR(void)
#elif defined(__GNUC__)
    void __attribute__ ((interrupt(USCI_A1_VECTOR))) USCI_A1_ISR (void)
#else
    #error Compiler not supported!
#endif
{
    // USART Tx Interrupt
    if(USCI_A_UART_getInterruptStatus(USCI_A1_BASE, USCI_A_UART_TRANSMIT_INTERRUPT_FLAG))
    {
        if(Usart2.tx_cnt < Usart2.tx_totle)
            UCA1TXBUF = Usart2.tx_buf[Usart2.tx_cnt++];
        else
            Usart2.tx_idle = 1;
        
        USCI_A_UART_clearInterrupt(USCI_A1_BASE, USCI_A_UART_TRANSMIT_INTERRUPT_FLAG);
    }
    
    // USART Rx Interrupt
    if(USCI_A_UART_getInterruptStatus(USCI_A1_BASE, USCI_A_UART_RECEIVE_INTERRUPT_FLAG))
    {
//        UCA1TXBUF = UCA1RXBUF;
        USCI_A_UART_clearInterrupt(USCI_A1_BASE, USCI_A_UART_RECEIVE_INTERRUPT_FLAG);
    }
}
