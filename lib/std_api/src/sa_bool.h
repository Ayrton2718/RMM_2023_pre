#ifndef STD_API_SRC_SA_BOOL_H
#define STD_API_SRC_SA_BOOL_H

#include "system/sa_type.h"


#ifdef __cplusplus
extern "C"{
#endif

typedef uint8_t SABool_t;

#define SABOOL_TRUE    ((SABool_t)1)
#define SABOOL_FALSE   ((SABool_t)0)

#define SABOOL_NOT(x) ((~((SABool_t)x)) & 0x01)

#define SABOOL_OR(x, y) ((x | y) & 0x01)
#define SABOOL_AND(x, y) ((x & y) & 0x01)

#ifdef __cplusplus
}
#endif

#endif /*STD_API_SRC_SA_BOOL_H*/