#ifndef __CONTROL_H__
#define __CONTROL_H__

#include "data_type.h"

#define  ANGLE_MAX      40.0                // 最大倾斜角度
#define  YAW_RATE_MAX   (180.0f/M_PI_F)     // 最大偏航速率

// 高度控制模式
typedef enum
{
    MANUAL = 0,
}FlylMode_t;

typedef struct
{
    float p;
    float i;
    float d;
    
    float set;
    float err;
    float pre_err;
    float integ;
    float i_limit;
    float derived;
    float out;
}PID_t;

extern PID_t PID_Pit_Pos;   // Pit 位置环 PID
extern PID_t PID_Pit_Spd;   // Pit 速度环 PID

extern PID_t PID_Rol_Pos;   // Rol 位置环 PID
extern PID_t PID_Rol_Spd;   // Rol 速度环 PID

extern PID_t PID_Yaw_Pos;   // Yaw 位置环 PID
extern PID_t PID_Yaw_Spd;   // Yaw 速度环 PID  

extern PID_t PID_Alt_Pos;   // Alt 位置环 PID
extern PID_t PID_Alt_Spd;   // Alt 速度环 PID

extern FlylMode_t altCtrlMode;
extern uint8 offLandFlag;

void Control_Update_Attitude_Angle(void);
void Control_Update_Attitude_Rate(void);
void Control_Update_Motor(void);
float Control_Estimate_Hover_Throttle(void);

#endif  // __CONTROL_H__

