#ifndef __ADC_H__
#define __ADC_H__

#include "data_type.h"

typedef enum
{
    ADC_Ch00 = 0,
    ADC_Ch01,
    ADC_Ch02,
    ADC_Ch03,
    ADC_Ch04,
    ADC_Ch05,
    ADC_Ch06,
    ADC_Ch07,
    ADC_Ch08,
    ADC_Ch09,
    ADC_Ch10,
    ADC_Ch11,
    ADC_Ch12,
    ADC_Ch13,
    ADC_Ch14,
    ADC_Ch15
}ADC_Ch_t;

extern void ADC_Init(void);
extern uint16 ADC_Ch(ADC_Ch_t ch);

#endif  // __ADC_H__

