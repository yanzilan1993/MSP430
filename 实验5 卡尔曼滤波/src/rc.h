#ifndef __RC_H__
#define __RC_H__

#include "data_type.h"

//RC遥控
typedef struct
{
    float Rol;
    float Pit;
    float Thr;
    float Yaw;
}RCProcessed_t;

typedef struct  
{
    int16 Start;        // 0xAAAA
    int16 ContrlBit;    // 0x01
    int16 Thr;          // 1000 - 2000
    int16 Pit;          // 1000 - 2000
    int16 Rol;          // 1000 - 2000
    int16 Yaw;          // 1000 - 2000
    int16 SW1;          // 1000 - 2000
    int16 SW2;          // 1000 - 2000
    int16 Left;         // 1000 - 2000
    int16 Right;        // 1000 - 2000
    int16 CaliFlag;     // 校正信号：01   非校正信号：00
    int16 Arm_State;    // ARM:5   DISARM:6
    int16 Stop;         // 0x5555
}RCPkg_t;

extern RCProcessed_t RCProcessed;
extern uint8_t FlyEnable;
extern RCPkg_t *RC_Data_Ptr;

void RC_Update_Raw(RCPkg_t *pRCPkg);
extern inline uint8 RC_Pkg_Decode(uint8 newByte);

#endif  // __RC_H__

