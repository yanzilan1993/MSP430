#ifndef __DBG_H__
#define __DBG_H__

#include "data_type.h"

typedef struct
{
    int16 ax;
    int16 ay;
    int16 az;
    
    int16 gx;
    int16 gy;
    int16 gz;
    
    float imu_rol;
    float imu_pit;
    float imu_yaw;
    
    float m_rol;
    float m_pit;
    float m_yaw;
    
    int16 rc_rol;
    int16 rc_pit;
    int16 rc_thr;
    int16 rc_yaw;
    
    float dbg_tar;
    float dbg_smp;
    float dbg_det;
}DbgInfo_t;

extern DbgInfo_t DbgInfo;

extern void Update_Debug_Message(void);

#endif  // __DBG_H__

