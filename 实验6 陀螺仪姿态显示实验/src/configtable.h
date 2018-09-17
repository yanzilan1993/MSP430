#ifndef __CONFIGTABLE_H__
#define __CONFIGTABLE_H__

#include "data_type.h"

typedef struct config_table_tt
{
    float version;
    float pidPitch[3];
    float pidPitchRate[3];
    float pidRoll[3];
    float pidRollRate[3];
    float pidYaw[3];
    float pidYawRate[3];
    float pidAlt[3];
    float pidAltVel[3];
    float accOffset[3];
    float gyrOffset[3];
    float magOffset[3];
    float NRFaddr[5];
    float BTstate;      //蓝牙是否需要重新写参数状态
    float NRFmatchFlag;
}config_table_t;

extern config_table_t table;

extern void LoadParamsFromEEPROM(void);
extern void ParamSetDefault(void) ;
extern void ParamToTable(void);

#endif  // __CONFIGTABLE_H__

