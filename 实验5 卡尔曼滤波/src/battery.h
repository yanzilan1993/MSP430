#ifndef __BATTERY_H__
#define __BATTERY_H__

#include "data_type.h"

extern void Battery_Init(void);
extern void Battery_Update_Alarm(void);
extern float Battery_Get_Voltage(void);

#endif  // __BATTERY_H__

