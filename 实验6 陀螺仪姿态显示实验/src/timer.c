#include "timer.h"
#include "timer_a.h"
#include "timer_b.h"
#include "data_type.h"
#include "led.h"
#include "usart.h"
#include "gpio.h"
#include "imu.h"
#include "mpu6050.h"

uint8 accUpdated = 0;

extern uint8 Loop_100_Hz_Flag = 0;
extern uint8 Loop_50_Hz_Flag = 0;
extern uint8 Loop_10_Hz_Flag = 0;
extern uint8 Loop_5_Hz_Flag = 0;

bool blEnInterupt = false;
volatile static uint64 System_ms_Cnt = 0;
    
volatile static uint16 PWM_Period_Cnt_Max = 500;
    
volatile static float PWM_Ch1_Duty = 0.20;
volatile static float PWM_Ch2_Duty = 0.40;
volatile static float PWM_Ch3_Duty = 0.60;
volatile static float PWM_Ch4_Duty = 0.80;
    
Timer_A_outputPWMParam Timer_A_outputPWMParam_Cache;

extern void Timer_A0_Init_PWM_Output(void)
{
    Timer_A_initUpModeParam Timer_A_initUpModeParam_cache;
    
    // Timer PWM GPIO Initialize
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN2);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN3);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN5);
    
    // Timer A0 1ms Period
    // 25 000 000 / 40 = 625 000
    Timer_A_initUpModeParam_cache.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
    Timer_A_initUpModeParam_cache.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_40;
    Timer_A_initUpModeParam_cache.timerPeriod = 625 - 1;
    Timer_A_initUpModeParam_cache.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
    Timer_A_initUpModeParam_cache.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE;
    Timer_A_initUpModeParam_cache.timerClear = TIMER_A_SKIP_CLEAR;
    Timer_A_initUpModeParam_cache.startTimer = true;
    Timer_A_initUpMode(TIMER_A0_BASE, &Timer_A_initUpModeParam_cache);
    
    PWM_Set_Ch_Duty(1, 0.80);
    PWM_Set_Ch_Duty(2, 0.60);
    PWM_Set_Ch_Duty(3, 0.40);
    PWM_Set_Ch_Duty(4, 0.20);
}

extern void Timer_B0_Init_ms_Interrupt(void)
{
    Timer_B_initUpModeParam Timer_B_initUpModeParam_cache;
    
    // Timer B0 1ms Period
    // 25 000 000 / 20 = 1 250 000
    Timer_B_initUpModeParam_cache.clockSource = TIMER_B_CLOCKSOURCE_ACLK;
    Timer_B_initUpModeParam_cache.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_20;
    Timer_B_initUpModeParam_cache.timerPeriod = 1250 - 1;
    Timer_B_initUpModeParam_cache.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_ENABLE;
    Timer_B_initUpModeParam_cache.captureCompareInterruptEnable_CCR0_CCIE = TIMER_B_CCIE_CCR0_INTERRUPT_ENABLE;
    Timer_B_initUpModeParam_cache.timerClear = TIMER_B_DO_CLEAR;
    Timer_B_initUpModeParam_cache.startTimer = true;
    Timer_B_initUpMode(TIMER_B0_BASE, &Timer_B_initUpModeParam_cache);
}

extern void PWM_Set_Ch_Duty(uint8 ch, float duty)
{
    if((ch<=4) && (duty>=0.00) && (duty<=1.00))
    {
        Timer_A_outputPWMParam_Cache.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
        Timer_A_outputPWMParam_Cache.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
        Timer_A_outputPWMParam_Cache.timerPeriod = PWM_Period_Cnt_Max;
        Timer_A_outputPWMParam_Cache.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
        
        switch(ch)
        {
            case 1: Timer_A_outputPWMParam_Cache.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;   break;
            case 2: Timer_A_outputPWMParam_Cache.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;   break;
            case 3: Timer_A_outputPWMParam_Cache.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3;   break;
            case 4: Timer_A_outputPWMParam_Cache.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_4;   break;
        }
        
        Timer_A_outputPWMParam_Cache.dutyCycle = (uint16)(PWM_Period_Cnt_Max * duty);
        Timer_A_outputPWM(TIMER_A0_BASE, &Timer_A_outputPWMParam_Cache);
    }
}

extern uint64 System_ms(void)
{
    return System_ms_Cnt;
}

extern uint64 System_us(void)
{
    // return (uint64)((System_ms_Cnt * 1000) + Timer_B_getCounterValue(TIMER_B0_BASE) * 1000 / 1250);
    return (uint64)((System_ms_Cnt * 1000) + Timer_B_getCounterValue(TIMER_B0_BASE) * 0.800f);
}

extern void delay_ms(uint64 ms)
{
    static uint64 StartCntVal = 0;
    
    StartCntVal = System_ms_Cnt;
    while((System_ms_Cnt - StartCntVal) < ms)
        ;
}

extern void delay_us(uint64 ms)
{
    uint64 Start_ms = System_ms();
    
    while((System_ms() - Start_ms) < ms)
        ;
}

extern uint64 millis(void)
{
    return System_ms();
}

extern uint64 micros(void)
{
    return System_us();
}

extern void Timer_Test(void)
{
    static uint32 Cnt = 0;
    
    Cnt++;
    
    if(Cnt < 50)
    {
        LED_On(&LED[LED_B]);
    }
    else if(Cnt < 1000)
    {
        LED_Off(&LED[LED_B]);
    }
    else
    {
        Cnt = 0;
    }
}

void PWM_Test(void)
{
    static uint32 ms = 0;
    static const float step = 0.001;
    static const float min = 0.00;
    static const float max = 1.00;
    
    if(++ms >= 5)
    {
        ms = 0;
        
        PWM_Ch1_Duty += step;
        PWM_Ch2_Duty += step;
        PWM_Ch3_Duty += step;
        PWM_Ch4_Duty += step;
        
        if(PWM_Ch1_Duty > max)
          PWM_Ch1_Duty = min;
        if(PWM_Ch2_Duty > max)
          PWM_Ch2_Duty = min;
        if(PWM_Ch3_Duty > max)
          PWM_Ch3_Duty = min;
        if(PWM_Ch4_Duty > max)
          PWM_Ch4_Duty = min;
        
        PWM_Set_Ch_Duty(1, PWM_Ch1_Duty);
        PWM_Set_Ch_Duty(2, PWM_Ch2_Duty);
        PWM_Set_Ch_Duty(3, PWM_Ch3_Duty);
        PWM_Set_Ch_Duty(4, PWM_Ch4_Duty);
    }
}

// Timer A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
    #pragma vector = TIMER0_A0_VECTOR
    __interrupt void TIMER0_A0_ISR(void)
#elif defined(__GNUC__)
    void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) TIMER0_A0_ISR (void)
#else
    #error Compiler not supported!
#endif
{
    Timer_A_clearTimerInterrupt(TIMER_A0_BASE);
}

// Timer B0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
    #pragma vector = TIMER0_B0_VECTOR
    __interrupt void TIMER0_B0_ISR(void)
#elif defined(__GNUC__)
    void __attribute__ ((interrupt(TIMER0_B0_VECTOR))) TIMER0_B0_ISR (void)
#else
    #error Compiler not supported!
#endif
{
    static uint8 Loop_100_Hz_Cnt = 0;
    static uint8 Loop_50_Hz_Cnt = 0;
    static uint8 Loop_10_Hz_Cnt = 0;
    static uint8 Loop_5_Hz_Cnt = 0;
    static uint8 Loop_6_ms_Cnt = 0;
    static uint8 Loop_3_ms_Cnt = 0;
    
    System_ms_Cnt++;
    
    if(blEnInterupt)
    {
      if(Loop_3_ms_Cnt++ > 2)
      {
          MpuGetData(); //获取MPU6050数据
          Loop_3_ms_Cnt = 0;
      }
      
      if(Loop_6_ms_Cnt++ > 5)
      {
          GetAngle(&MPU6050,&Angle,0.00626f); 
          Loop_6_ms_Cnt = 0;
      }
    }
    
    
    if(Loop_100_Hz_Cnt++ >= 10)
    {
        Loop_100_Hz_Cnt = 0;
        Loop_100_Hz_Flag = 1;
    }
    
    if(Loop_50_Hz_Cnt++ >= 20)
    {
        Loop_50_Hz_Cnt = 0;
        Loop_50_Hz_Flag = 1;
    }
    
    if(Loop_10_Hz_Cnt++ >= 100)
    {
        Loop_10_Hz_Cnt = 0;
        Loop_10_Hz_Flag = 1;
    }
    
    if(Loop_5_Hz_Cnt++ >= 200)
    {
        Loop_5_Hz_Cnt = 0;
        Loop_5_Hz_Flag = 1;
        
       // printf("The ACC_X = %f.2",MPU6050.accX);
        
        int16_t ACC_X = (int16_t)MPU6050.accX;
        U1_UCA0_Send((uint8_t*)ACC_X, 2);
    }
    
//    Timer_Test();
//    PWM_Test();
    
    Timer_B_clearTimerInterrupt(TIMER_B0_BASE);
}

