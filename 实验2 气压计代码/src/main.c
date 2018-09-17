#include "board.h"
#include "usart.h"
#include "timer.h"
#include "led.h"
#include "mpu6050.h"
#include "spl06.h"

/*请发送这个数值*/
extern float height;

int main(void)
{
    
    System_Clock_Init();
    
    _EINT();

    Timer_B0_Init_ms_Interrupt();
    
    U1_UCA0_Init(USCI_A_UART_CLOCKSOURCE_ACLK, 57600);
    U2_UCA1_Init(USCI_A_UART_CLOCKSOURCE_ACLK, 115200);

    SPL06_init();
    LED_Init();
    
    delay_ms(500);
    
    for( ; ; )
    {
        user_SPL06_get();
        
        for(int i=0;i<4;i++)
        {
            LED_Toogle(&LED[i+2]);
        }
        
        delay_ms(100);
    }
}
