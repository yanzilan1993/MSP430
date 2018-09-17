#ifndef __DATATYPE_H__
#define __DATATYPE_H__

#include "stdint.h"

typedef __INT8_T_TYPE__     int8;       // -128                 ~ 127
typedef __UINT8_T_TYPE__    uint8;      // 0                    ~ 255

typedef __INT16_T_TYPE__    int16;      // -32768               ~ 32767
typedef __UINT16_T_TYPE__   uint16;     // 0                    ~ 65535

typedef __INT32_T_TYPE__    int32;      // -2147483648          ~ 2147483647
typedef __UINT32_T_TYPE__   uint32;     // 0                    ~ 4294967295

typedef __INT64_T_TYPE__    int64;      // -9223372036854775808 ~ 9223372036854775807
typedef __UINT64_T_TYPE__   uint64;     // 0                    ~ 18446744073709551615

#define UINT16_BYTE_L(x)        ((uint8)(x))
#define UINT16_BYTE_H(x)        ((uint8)((x) >> 8))

#endif  // __DATATYPE_H__

