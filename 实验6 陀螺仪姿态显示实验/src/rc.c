#include "rc.h"
#include "rc.h"
#include "commapp.h"
#include "imu.h"
#include "led.h"
#include "usart.h"
#include "sysconfig.h"
#include "motor.h"
#include "dbg.h"

#define UNLOCK_THR_MAX     1200

extern uint8 FlyEnable = 0;

RCProcessed_t RCProcessed;

void RC_Update_Raw(RCPkg_t *pRCPkg)
{
    typedef volatile int16 dbg_int16;
    
    dbg_int16 start, stop, check, rol, pit, thr, yaw;
    
    start = pRCPkg->Start;
    stop = pRCPkg->Stop;
    check = pRCPkg->ContrlBit;
    rol = pRCPkg->Rol;
    pit = pRCPkg->Pit;
    thr = pRCPkg->Thr;
    yaw = pRCPkg->Yaw;
    
    RCRaw[Rol] = pRCPkg->Rol;
    RCRaw[Pit] = pRCPkg->Pit;
    RCRaw[Thr] = pRCPkg->Thr;
    RCRaw[Yaw] = pRCPkg->Yaw;
    
    DbgInfo.rc_rol = pRCPkg->Rol;
    DbgInfo.rc_pit = pRCPkg->Pit;
    DbgInfo.rc_thr = pRCPkg->Thr;
    DbgInfo.rc_yaw = pRCPkg->Yaw;
    
    if(pRCPkg->Arm_State == 1)
    {
        if(ArmState == LockSta_Locked)
        {
            if(pRCPkg->Thr < UNLOCK_THR_MAX)
            {
                ArmState = LockSta_Unlock;
                FlyEnable = 1;
            }
        }
        else //if(ArmState == LockSta_Unlock)
        {
            ArmState = LockSta_Locked;
            FlyEnable = 0;
            MotorPwmFlash(0, 0, 0, 0);
        }
    }
    
    if(pRCPkg->CaliFlag == 1)
    {
        imuCaliFlag = 1;
    }
}

extern inline uint8 RC_Pkg_Decode(uint8 newByte)
{
    static uint8 cnt = 0;
        
    switch(cnt)
    {
        case 0:
        case 1:
        {
            if(0xAA != newByte)
            {
                cnt = 0;
                return 0;
            }
        }
        break;
        
        case 2:
        {
            if(0x01 != newByte)
            {
                cnt = 0;
                return 0;
            }
        }
        break;
        
        case 3:
        {
            if(0x00 != newByte)
            {
                cnt = 0;
                return 0;
            }
        }
        break;
        
        case 24:
        case 25:
        {
            if(0x55 != newByte)
            {
                cnt = 0;
                return 0;
            }
        }
        break;
    }
    
    Usart1.rx_buf[cnt++] = newByte;
    
    if(cnt >= sizeof(RCPkg_t))
    {
        LED_Toogle(&LED[LED_R]);
        cnt = 0;
        
        RC_Update_Raw((RCPkg_t*)Usart1.rx_buf);
        RC_Update_Process();
        
        return 1;
    }
    
    return 0;
}
