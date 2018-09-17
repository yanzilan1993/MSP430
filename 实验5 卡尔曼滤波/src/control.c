#include "control.h"
#include "extern_variable.h"
#include <math.h>
#include "imu.h"
#include "rc.h"
#include "battery.h"
#include "altitude.h"
#include "motor.h"
#include "timer.h"
#include "sysconfig.h"
#include "led.h"
#include "dbg.h"

extern uint8 offLandFlag = 0;
extern FlylMode_t altCtrlMode = MANUAL;

static int16 MotorPWM[4] = {0};

static float Thr_Out = 0.0;
static float Rol_Out = 0.0;
static float Pit_Out = 0.0;
static float Yaw_Out = 0.0;

PID_t PID_Pit_Pos = {0};  // pitch角度环的PID
PID_t PID_Pit_Spd = {0};  // pitch角速率环的PID

PID_t PID_Rol_Pos = {0};  // roll角度环的PID
PID_t PID_Rol_Spd = {0};  // roll角速率环的PID

PID_t PID_Yaw_Pos = {0};  // yaw角度环的PID
PID_t PID_Yaw_Spd = {0};  // yaw角速率环的PID

PID_t	PID_Alt_Pos = {0};
PID_t PID_Alt_Spd = {0};

S_FLOAT_XYZ DIF_ACC;    // 实际去期望相差的加速度
S_FLOAT_XYZ EXP_ANGLE;	// 期望角度
S_FLOAT_XYZ DIF_ANGLE;	// 实际与期望相差的角度

uint32 ctrlPrd = 0;
float headHold = 0;

static void PID_Calculate(PID_t * PID, float target, float measure, uint64 dertT)
{
    float termI = 0;
    float dt = dertT / 1000000.0;

    //误差=期望值-测量值
    PID->err=target-measure;

    PID->derived= (PID->err-PID->pre_err)/dt;

    PID->out=(PID->p * PID->err) + (PID->i * PID->integ) + (PID->d * PID->derived);

    PID->pre_err=PID->err;
    
    //仅用于角度环和角速度环的
    if(FlyEnable && offLandFlag)
    {
        if(fabs(PID->out) < Thr_Out )               // 比油门还大时不积分
        {
            termI = PID->integ + PID->err * dt;   // 积分环节
            if(termI > -PID->i_limit && termI < PID->i_limit && PID->out > -PID->i_limit && PID->out < PID->i_limit)  // 在-300~300时才进行积分环节
                PID->integ = termI;
        }
    }
    else
    {
        PID->integ = 0;
    }
}

// 函数名：CtrlAttiAng(void)
// 描述：对飞行器姿态控制（pitch，roll，yaw）控制中，串级PID中的角度环控制
void Control_Update_Attitude_Angle(void)
{
    static uint64 tPrev = 0;
    float angTarget[3] = {0};
    uint64 dt = 0;
    uint64 t = 0;
    
    t = micros();
    dt = (tPrev>0) ? (t-tPrev) : 0;
    tPrev = t;
    
    angTarget[Rol] = (RCProcessed.Rol);
    angTarget[Pit] = (RCProcessed.Pit);
    
    PID_Calculate(&PID_Pit_Pos, angTarget[Pit], imu.pit_deg, dt);
    PID_Calculate(&PID_Rol_Pos, angTarget[Rol], imu.rol_deg, dt);
}

// 函数名：CtrlAttiRate(void)
// 描述：对飞行器姿态控制（pitch，roll，yaw）控制中，串级PID中的角速度环控制
void Control_Update_Attitude_Rate(void)
{
    float YawRateTarget = 0;
    static uint64 tPrev = 0;
    
	float gyro_pit = imu.gyro[Pit] - imu.gyrOffset[Pit];
	float gyro_rol = imu.gyro[Rol] - imu.gyrOffset[Rol];
	float gyro_yaw = imu.gyro[Yaw] - imu.gyrOffset[Yaw];
	
    uint64 dt = 0;
    uint64 t = 0;
    t = micros();
    dt = (tPrev>0) ? (t-tPrev) : 0;
    tPrev = t;

    YawRateTarget = -RCProcessed.Yaw;
    
    // 注意，原来的pid参数，对应的是 ad值,故转之
#ifdef IMU_SW
	if(fabs(gyro_pit) < 0.001)
		gyro_pit = 0;
    
	if(fabs(gyro_rol) < 0.001)
		gyro_rol = 0;
    
	if(fabs(gyro_yaw) < 0.001)
		gyro_yaw = 0;
    
    // 计算PID参数
    PID_Calculate(&PID_Pit_Spd, PID_Pit_Pos.out, gyro_pit*180.0f/M_PI_F, dt);
    PID_Calculate(&PID_Rol_Spd, PID_Rol_Pos.out, gyro_rol*180.0f/M_PI_F, dt);
    PID_Calculate(&PID_Yaw_Spd, YawRateTarget, gyro_yaw*180.0f/M_PI_F, dt);
#else

    // 原参数对应于 DMP的直接输出gyro , 是deg.  且原DMP之后的处理运算是错误的
    PID_Calculate(&PID_Pit_Spd, PID_Pit_Pos.out, imu.gyro[Pit]*DMP_GYRO_SCALE, 0);
    PID_Calculate(&PID_Rol_Spd, PID_Rol_Pos.out, imu.gyro[Rol]*DMP_GYRO_SCALE, 0);
    PID_Calculate(&PID_Yaw_Spd, YawRateTarget, imu.gyro[Yaw]*DMP_GYRO_SCALE, 0);
#endif

    // 更新计算好的PID输出量
    Pit_Out = PID_Pit_Spd.out;
    Rol_Out = PID_Rol_Spd.out;
    Yaw_Out = PID_Yaw_Spd.out;
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

// Get a estimated value for hold throttle.
float Control_Estimate_Hover_Throttle(void)
{
    float hoverHru = -0.55f;
    float bat_voltage = Battery_Get_Voltage();

    if(bat_voltage > 4.05)
        hoverHru = -0.25f;
    else if(bat_voltage > 3.90)
        hoverHru = -0.40f;
    else if(bat_voltage > 3.80)
        hoverHru = -0.45f;
    else if(bat_voltage > 3.70)
        hoverHru = -0.50f;
    else
        hoverHru = -0.55f;

    return hoverHru;
}

void Control_Update_Motor(void)
{

    /*****************************
    Sequence of Motors

        M2              M0
         \             /
          \           /

          msp430_drone

          /           \
         /             \
        M1              M3

    ****************************/
    Thr_Out = RCProcessed.Thr;
    
    MotorPWM[0] = (int16)(Thr_Out - Pit_Out - Rol_Out - Yaw_Out );
    MotorPWM[1] = (int16)(Thr_Out + Pit_Out + Rol_Out - Yaw_Out );
    MotorPWM[2] = (int16)(Thr_Out - Pit_Out + Rol_Out + Yaw_Out );
    MotorPWM[3] = (int16)(Thr_Out + Pit_Out - Rol_Out + Yaw_Out );
    
    DbgInfo.m_rol = Rol_Out;
    DbgInfo.m_pit = Pit_Out;
    DbgInfo.m_yaw = Yaw_Out;
    
    if(FlyEnable)
    {
        MotorPwmFlash(MotorPWM[0], MotorPWM[1], MotorPWM[2], MotorPWM[3]);
        LED_On(&LED[LED_B]);
    }
    else
    {
        MotorPwmFlash(0, 0, 0, 0);
        LED_Off(&LED[LED_B]);
    }
}