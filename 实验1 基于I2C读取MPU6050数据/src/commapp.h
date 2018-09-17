#ifndef __COMMAPP_H__
#define __COMMAPP_H__

#include "data_type.h"
#include "imu.h"

typedef enum
{
    LockSta_Unlock = 0,         // 已解锁
    LockSta_Locked,             // 已锁定
}LockSta_t;

#define APP_YAW_DB          1   // dead band
#define APP_PR_DB           1

// 控制源类型 无控制 遥控器 App
typedef enum
{
    CtrSrc_No,                  // 无控制
    CtrSrc_RC,                  // 遥控器
}CtrSrc_t;

extern int16 RCRaw[IMU_Axis_Num];
extern uint64 lastGetStickTime;
extern uint64 lastGetAppTime;
extern LockSta_t ArmState;
extern CtrSrc_t stickOrAppControl;

void RC_Update_Process(void);

#endif  // __COMMAPP_H__

