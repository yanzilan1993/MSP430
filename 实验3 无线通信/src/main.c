#include "board.h"
#include "usart.h"
#include "timer.h"
#include "led.h"
#include "motor.h"
#include "mpu6050.h"
#include "kalman.h"
#include "nrf24l01.h"

int main(void)
{
    System_Clock_Init();
    
    _EINT();

    Timer_B0_Init_ms_Interrupt();

    NRF24L01_Init();
    
    delay_ms(500);
    
    for( ; ; )
    {

        delay_ms(500);
    }
}
