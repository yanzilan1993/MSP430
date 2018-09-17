#ifndef __LED_H__
#define __LED_H__

#include "data_type.h"

typedef enum
{
    LED_B = 0,
    LED_R,
    LED_M1,
    LED_M2,
    LED_M3,
    LED_M4,
    
    LED_Num,
}LED_Item_t;

typedef struct
{
    uint16 Pin;
    uint8 Port;
    uint8 Level_On;
}LED_t;

/*从原STM32工程中移植而来*/
typedef struct
{
	uint16_t FlashTime;
	enum
	{
		AlwaysOn, 
		AlwaysOff, 
		AllFlashLight,
		AlternateFlash, 
		WARNING,
		DANGEROURS,
		GET_OFFSET	
	}status; 
}sLED;	

//extern sLED LED;
extern LED_t LED[LED_Num];

extern void LED_Init(void);
extern void LED_On(LED_t* LED_x);
extern void LED_Off(LED_t* LED_x);
extern void LED_Toogle(LED_t* LED_x);
extern void LED_Set(LED_t* LED_x, uint8 NewSta);

extern void LEDFSM(void);

extern void LED_Test(void);

#endif  // __LED_H__

