#ifndef __TIMER_H__
#define __TIMER_H__

#include "data_type.h"
#include "stdbool.h"

extern uint8 Loop_100_Hz_Flag;
extern uint8 Loop_50_Hz_Flag;
extern uint8 Loop_10_Hz_Flag;
extern uint8 Loop_5_Hz_Flag;
extern bool blEnInterupt;

extern void Timer_A0_Init_PWM_Output(void);
extern void Timer_B0_Init_ms_Interrupt(void);
extern void PWM_Set_Ch_Duty(uint8 ch, float duty);
extern uint64 System_ms(void);
extern uint64 System_us(void);
extern void delay_ms(uint64 ms);

extern uint64 micros(void);
extern uint64 millis(void);

extern void Timer_Test(void);
extern void PWM_Test(void);

#endif  // __TIMER_H__

