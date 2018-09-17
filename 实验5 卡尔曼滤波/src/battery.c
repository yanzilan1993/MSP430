#include "battery.h"
#include "adc.h"
#include "led.h"

static float BatteryVoltage = 0.0;

extern void Battery_Init()
{
    ADC_Init();
}

extern void Battery_Update_Alarm(void)
{
    float bat_adc = 0.0;
    float bat_vol = 0.0;
    
    bat_adc = ADC_Ch(ADC_Ch00);
    
    bat_vol = (bat_adc * 2 * 3.3) / 4096;
    BatteryVoltage = bat_vol;
    
    if(bat_vol < 3.6)
    {
        LED_On(&LED[LED_M1]);
        LED_On(&LED[LED_M2]);
        LED_On(&LED[LED_M3]);
        LED_On(&LED[LED_M4]);
    }
    else
    {
        LED_Off(&LED[LED_M1]);
        LED_Off(&LED[LED_M2]);
        LED_Off(&LED[LED_M3]);
        LED_Off(&LED[LED_M4]); 
    }
}

extern float Battery_Get_Voltage(void)
{
    return BatteryVoltage;
}
