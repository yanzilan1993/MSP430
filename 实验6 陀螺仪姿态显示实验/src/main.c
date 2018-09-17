#include "board.h"
#include "usart.h"
#include "timer.h"
#include "led.h"
#include "motor.h"
#include "mpu6050.h"
#include "imu.h"
#include "i2c.h"
int main(void)
{   
    uint8_t Test_Buff[5] = {0xAA,0xBB,0xCC,0xDD,0xEE};
    
    System_Clock_Init();
    
    _EINT();
    
    Timer_B0_Init_ms_Interrupt();
    
    U1_UCA0_Init(USCI_A_UART_CLOCKSOURCE_ACLK, 57600);
    U2_UCA1_Init(USCI_A_UART_CLOCKSOURCE_ACLK, 115200);

    delay_ms(500);
    
    MpuInit(); //≥ı ºªØ
    
    for( ; ; )
    {
        
    }
}
