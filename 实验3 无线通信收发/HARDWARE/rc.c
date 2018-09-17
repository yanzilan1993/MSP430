#include "rc.h"
#include "usart.h"
#include "Motor.h"
#include "led.h"
#include "ALL_DEFINE.h"

#define UNLOCK_THR_MAX     1200

extern uint8 FlyEnable = 0;
extern int16 RCRaw[4] = {1500, 1500, 1500, 1500};
RCProcessed_t RCProcessed;

float dbScaleLinear(float x, float x_end, float deadband);
void RC_Update_Process(void);
#define APP_YAW_DB          1   // dead band
#define APP_PR_DB           1

#define M_PI_F 3.1415926
#define  YAW_RATE_MAX   (180.0f/M_PI_F)     // 最大偏航速率
#define  ANGLE_MAX      40.0                // 最大倾斜角度

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
    
    //---------------------------------------------------------------------------------
    //开始处理遥控数据
    {
        const float roll_pitch_ratio = 0.04f;  //遥控控制姿态的量
        
        Remote.roll = RCRaw[Rol];

        Remote.pitch = RCRaw[Pit];

        Remote.thr = RCRaw[Thr];

        Remote.yaw = RCRaw[Yaw];

        pidPitch.desired =-(Remote.pitch-1500)*roll_pitch_ratio;	 
        pidRoll.desired = -(Remote.roll-1500)*roll_pitch_ratio;
        
        if(Remote.yaw>1820 )
        {							
          pidYaw.desired += 0.45f;	
        }
        else if(Remote.yaw <1180)
        {
          pidYaw.desired -= 0.45f;	
        }
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

float dbScaleLinear(float x, float x_end, float deadband)
{
    if(x > deadband)
    {
        return (x - deadband) / (x_end - deadband);
    }
    else if(x < -deadband)
    {
        return (x + deadband) / (x_end - deadband);
    }
    else
    {
        return 0;
    }
}
