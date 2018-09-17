#include "commapp.h"
#include "imu.h"
#include "rc.h"
#include "control.h"
#include "uart1.h"
#include "altitude.h"
#include "battery.h"
#include "timer.h"

#define CONSTRAIN(x, min, max)  { if((x) < (min))   x = (min);\
                                  if((x) > (max))   x = (max); }

extern int16 RCRaw[4] = {1500, 1500, 1500, 1500};
extern uint64 lastGetStickTime = 0;
extern uint64 lastGetAppTime = 0;
extern LockSta_t ArmState = LockSta_Unlock;
extern CtrSrc_t stickOrAppControl = CtrSrc_RC;

extern float dbScaleLinear(float x, float x_end, float deadband);

void RC_Update_Process(void)
{
    // 对遥控器的值限幅
    CONSTRAIN(RCRaw[Thr], 1000, 2000);
    CONSTRAIN(RCRaw[Yaw], 1000, 2000);
    CONSTRAIN(RCRaw[Pit], 1000, 2000);
    CONSTRAIN(RCRaw[Rol], 1000, 2000);

    // 对遥控器的4个通道进行死区限制
    RCProcessed.Thr = RCRaw[Thr] - 1000;
    RCProcessed.Yaw = YAW_RATE_MAX * dbScaleLinear((RCRaw[Yaw] - 1500), 500, APP_YAW_DB);
    RCProcessed.Pit = ANGLE_MAX * dbScaleLinear((RCRaw[Pit] - 1500), 500, APP_PR_DB);
    RCProcessed.Rol = ANGLE_MAX * dbScaleLinear((RCRaw[Rol] - 1500), 500, APP_PR_DB);
}
