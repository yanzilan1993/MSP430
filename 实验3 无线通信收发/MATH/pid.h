
#ifndef __PID_H
#define __PID_H
#include "ALL_DATA.h"
#include <stdbool.h>

extern void CascadePID(PidObject* pidRate,PidObject* pidAngE,const float dt);  //����PID
extern void pidRest(PidObject **pid,const uint8_t len); //pid���ݸ�λ
extern void pidUpdate(PidObject* pid,const float dt);  //PID
#endif



