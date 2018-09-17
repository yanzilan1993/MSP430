#include "led.h"
#include "gpio.h"

// LED_B P2.0
#define LED_B_PORT          GPIO_PORT_P2
#define LED_B_PIN           GPIO_PIN0
#define LED_B_LEVEL_ON      0

// LED_R P2.1
#define LED_R_PORT          GPIO_PORT_P2
#define LED_R_PIN           GPIO_PIN1
#define LED_R_LEVEL_ON      0

// LED_M1                   P2.4
#define LED_M1_PORT         GPIO_PORT_P2
#define LED_M1_PIN          GPIO_PIN4
#define LED_M1_LEVEL_ON     0

// LED_M2 P2.5
#define LED_M2_PORT         GPIO_PORT_P2
#define LED_M2_PIN          GPIO_PIN5
#define LED_M2_LEVEL_ON     0

// LED_M3 P2.6
#define LED_M3_PORT         GPIO_PORT_P2
#define LED_M3_PIN          GPIO_PIN6
#define LED_M3_LEVEL_ON     0

// LED_M4 P2.7
#define LED_M4_PORT         GPIO_PORT_P2
#define LED_M4_PIN          GPIO_PIN7
#define LED_M4_LEVEL_ON     0

extern LED_t LED[LED_Num] = {0};
//sLED LED = {300,AllFlashLight};  //LED initial statue is off;

extern void LED_Init(void)
{
    uint8 i;
    
    for(i=0; i<LED_Num; i++)
    {
        LED[i].Port = GPIO_PORT_P2;
        LED[i].Pin = GPIO_PIN0;
        LED[i].Level_On = 0;
    }
    
    LED[LED_B].Pin = GPIO_PIN0;
    LED[LED_R].Pin = GPIO_PIN1;
    LED[LED_M1].Pin = GPIO_PIN4;
    LED[LED_M2].Pin = GPIO_PIN5;
    LED[LED_M3].Pin = GPIO_PIN6;
    LED[LED_M4].Pin = GPIO_PIN7;
    
    for(i=0; i<LED_Num; i++)
    {
        GPIO_setAsOutputPin(LED[i].Port, LED[i].Pin);
        GPIO_setDriveStrength(LED[i].Port, LED[i].Pin, GPIO_FULL_OUTPUT_DRIVE_STRENGTH);
        LED_Off(&LED[i]);
    }
    
    LED_Test();
}

extern void LED_On(LED_t* LED_x)
{
    if(LED_x->Level_On)
        GPIO_setOutputHighOnPin(LED_x->Port, LED_x->Pin);
    else
        GPIO_setOutputLowOnPin(LED_x->Port, LED_x->Pin);
}
extern void LED_Off(LED_t* LED_x)
{
    if(LED_x->Level_On)
        GPIO_setOutputLowOnPin(LED_x->Port, LED_x->Pin);
    else
        GPIO_setOutputHighOnPin(LED_x->Port, LED_x->Pin);
}

extern void LED_Toogle(LED_t* LED_x)
{
    GPIO_toggleOutputOnPin(LED_x->Port, LED_x->Pin);
}

extern void LED_Set(LED_t* LED_x, uint8 NewSta)
{
    if(NewSta)
        GPIO_setOutputHighOnPin(LED_x->Port, LED_x->Pin);
    else
        GPIO_setOutputLowOnPin(LED_x->Port, LED_x->Pin);
}

extern void LEDFSM(void)
{

}

extern void LED_Test(void)
{
    uint8 i;
    
    for(i=0; i<LED_Num; i++)
    {
        LED_On(&LED[i]);
        LED_Off(&LED[i]);
        
        LED_On(&LED[i]);
        LED_Off(&LED[i]);
        
        LED_Toogle(&LED[i]);
        LED_Toogle(&LED[i]);
    }
    
    LED_On(&LED[LED_M1]);
    LED_On(&LED[LED_M2]);
    LED_On(&LED[LED_M3]);
    LED_On(&LED[LED_M4]);
    
    LED_Off(&LED[LED_M1]);
    LED_Off(&LED[LED_M2]);
    LED_Off(&LED[LED_M3]);
    LED_Off(&LED[LED_M4]);
}