#include "dbg.h"
#include "usart.h"
#include "timer.h"

extern DbgInfo_t DbgInfo = {0};

uint64 ms = 0;

extern void Update_Debug_Message(void)
{
    static uint64 ms_last;
    static uint16 ms;
    
    ms = System_ms() - ms_last;
    ms_last = System_ms();
    
//    sprintf((char*)Usart2.tx_buf,
//            "%+5dms a[%+6dx %+6dy %+6dz] g[%+6dx %+6dy %+6dz] IMU[%+7.2fr %+7.2fp %+7.2fy] M[%+9.2fR %+9.2fP %+9.2fY] RC[%+6dr %+6dp %+6dt %+6dy]\r\n",
//            ms,
//            DbgInfo.ax,
//            DbgInfo.ay,
//            DbgInfo.az,
//            DbgInfo.gx,
//            DbgInfo.gy,
//            DbgInfo.gz,
//            DbgInfo.imu_rol,
//            DbgInfo.imu_pit,
//            DbgInfo.imu.yaw_deg,
//            DbgInfo.m_rol,
//            DbgInfo.m_pit,
//            DbgInfo.m_yaw,
//            DbgInfo.rc_rol,
//            DbgInfo.rc_pit,
//            DbgInfo.rc_thr,
//            DbgInfo.rc_yaw
//            );
    
    sprintf((char*)Usart2.tx_buf,
            "%+5dms IMU[%+7.2fr %+7.2fp %+7.2fy] M[%+9.2fR %+9.2fP %+9.2fY] RC[%+6dr %+6dp %+6dt %+6dy] [%+9.2fTar %+9.2fSmp %+9.2fDet]\r\n",
            ms,
            
            DbgInfo.imu_rol,
            DbgInfo.imu_pit,
            DbgInfo.imu_yaw,
            
            DbgInfo.m_rol,
            DbgInfo.m_pit,
            DbgInfo.m_yaw,
            
            DbgInfo.rc_rol,
            DbgInfo.rc_pit,
            DbgInfo.rc_thr,
            DbgInfo.rc_yaw,
            
            DbgInfo.dbg_tar,
            DbgInfo.dbg_smp,
            DbgInfo.dbg_det
            );
    
    U2_UCA1_Send(Usart2.tx_buf, strlen((char const*)Usart2.tx_buf));
}