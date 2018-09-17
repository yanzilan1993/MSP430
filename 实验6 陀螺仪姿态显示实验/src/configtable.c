#include "configtable.h"
#include "nrf24l01.h"
#include "control.h"
#include "imu.h"

#define EEPROM_DEFAULT_VERSION  1   // ZKHD：默认版本为1

extern config_table_t table = {0};  // 用来存放EEPROM列表上的存放的参数变量的信息

extern void ParamToTable(void)
{
    uint8 i = 0;
    
    for(i=0; i<3; i++)
    {
        ((float *)(&table.pidPitch))[i] = ((float *)(&PID_Pit_Pos))[i];
        *((float *)(&table.pidRoll) + i) =  ((float *)(&PID_Rol_Pos))[i];
        ((float *)(&table.pidRoll))[i] = ((float *)(&PID_Rol_Pos))[i];
        ((float *)(&table.pidYaw))[i] = ((float *)(&PID_Yaw_Pos))[i];
        
        ((float *)(&table.pidPitchRate))[i] = ((float *)(&PID_Pit_Spd))[i];
        ((float *)(&table.pidRollRate))[i] = ((float *)(&PID_Rol_Spd))[i];
        ((float *)(&table.pidYawRate))[i] = ((float *)(&PID_Yaw_Spd))[i];
        
        ((float *)(&table.pidAlt))[i] = ((float *)(&PID_Alt_Pos))[i];
        ((float *)(&table.pidAltVel))[i] = ((float *)(&PID_Alt_Spd))[i];
        
        table.accOffset[i] = imu.accOffset[i];
        table.gyrOffset[i] = imu.gyrOffset[i];
    }

    for(i=0; i<5; i++)
    {
        ((float *)(&table.NRFaddr))[i] = ((uint8 *)(&RX_ADDRESS))[i];
    }
}

extern void LoadParamsFromEEPROM(void)
{
    ParamSetDefault();                      // 版本检测不对，各项参数设为默认值
    ParamToTable();                         // 把参数更新到控制器中
    table.version = EEPROM_DEFAULT_VERSION; // 更新默认版本信息
}

extern void ParamSetDefault(void)
{
    PID_Pit_Pos.p = 3.5;
    PID_Pit_Pos.i = 0;
    PID_Pit_Pos.d = 0;

    PID_Pit_Pos.i_limit = 300;

    PID_Pit_Spd.p  = 0.7;
    PID_Pit_Spd.i  = 0.5;
    PID_Pit_Spd.d  = 0.03;

    PID_Pit_Spd.i_limit = 300;
    
    PID_Rol_Pos.p = 3.5;
    PID_Rol_Pos.i = 0;
    PID_Rol_Pos.d = 0;
    PID_Rol_Pos.i_limit = 300;

    PID_Rol_Spd.p  = 0.7;
    PID_Rol_Spd.i  = 0.5;
    PID_Rol_Spd.d  = 0.03;
    PID_Rol_Spd.i_limit = 300;
    
    PID_Yaw_Pos.p = 1;
    PID_Yaw_Pos.i = 0.2;
    PID_Yaw_Pos.d = 0;

    PID_Yaw_Spd.p = 10;
    PID_Yaw_Spd.i = 0;
    PID_Yaw_Spd.d = 0;
    
    PID_Alt_Pos.p = 1.0;
    PID_Alt_Pos.i = 0;
    PID_Alt_Pos.d = 0;
    
    PID_Alt_Spd.p = 0.1f;
    PID_Alt_Spd.i = 0.02f;
    PID_Alt_Spd.d = 0;
    
    imu.accOffset[0] = -0.1620515;
    imu.accOffset[1] = 0.07422026;
    imu.accOffset[2] = 0.7743073;

    imu.gyrOffset[0] = -0.06097556;
    imu.gyrOffset[1] = -0.03780485;
    imu.gyrOffset[2] = 0;
}
