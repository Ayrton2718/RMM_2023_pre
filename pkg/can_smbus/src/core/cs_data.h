#ifndef CAN_SMBUS_SRC_CORE_CS_DATA_H
#define CAN_SMBUS_SRC_CORE_CS_DATA_H

#include "io/cs_type.h"


// #ifdef _FLT_NO_DENORMALS

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    CSData_type_None = 0,

    CSData_type_Buff1 = 1,
    CSData_type_Buff2 = 2,
    CSData_type_Buff3 = 3,
    CSData_type_Buff4 = 4,

	CSData_type_Int32,
	CSData_type_Int16,
	CSData_type_Int8,

    CSData_type_Uint32,
	CSData_type_Uint16,
	CSData_type_Uint8,

    CSData_type_Float,

	CSData_type_Bool,

    CSData_type_Null
} CSData_type_t;


typedef union
{
    int32_t Int32;
    int16_t Int16;
    int8_t Int8;

    uint32_t Uint32;
    uint16_t Uint16;
    uint8_t Uint8;

    float Float;

    SABool_t Bool;

    uint8_t Buff[4];
} CSData_var_t;


typedef struct
{
    CSData_var_t var;
    CSData_type_t type;
} CSData_t;


static inline size_t CSData_getSize(const CSData_t* data)
{
    switch(data->type)
    {
    case CSData_type_Int32:
    case CSData_type_Uint32:
    case CSData_type_Float:
    case CSData_type_Buff4:
        return 4;

    case CSData_type_Buff3:
        return 3;

    case CSData_type_Int16:
    case CSData_type_Uint16:
    case CSData_type_Buff2:
        return 2;

    case CSData_type_Int8:
    case CSData_type_Uint8:
    case CSData_type_Bool:
    case CSData_type_Buff1:
        return 1;
    
    case CSData_type_Null:
        return 0;
    
    case CSData_type_None:
    default:
        SALOG_ERROR("can_smbus", "Invalid data type!");
        return 0;
    }
}


static inline CSData_t CSData_createInt32(int32_t init_value)
{
    CSData_t data;
    data.type = CSData_type_Int32;
    data.var.Int32 = init_value;
    return data;
}

static inline CSData_t CSData_createInt16(int16_t init_value)
{
    CSData_t data;
    data.type = CSData_type_Int16;
    data.var.Int16 = init_value;
    return data;
}

static inline CSData_t CSData_createInt8(int8_t init_value)
{
    CSData_t data;
    data.type = CSData_type_Int8;
    data.var.Int8 = init_value;
    return data;
}


static inline CSData_t CSData_createUint32(uint32_t init_value)
{
    CSData_t data;
    data.type = CSData_type_Uint32;
    data.var.Uint32 = init_value;
    return data;
}

static inline CSData_t CSData_createUint16(uint16_t init_value)
{
    CSData_t data;
    data.type = CSData_type_Uint16;
    data.var.Uint16 = init_value;
    return data;
}

static inline CSData_t CSData_createUint8(uint8_t init_value)
{
    CSData_t data;
    data.type = CSData_type_Uint8;
    data.var.Uint8 = init_value;
    return data;
}


static inline CSData_t CSData_createFloat(float init_value)
{
    CSData_t data;
    data.type = CSData_type_Float;
    data.var.Float = init_value;
    return data;
}

static inline CSData_t CSData_createBool(SABool_t init_value)
{
    CSData_t data;
    data.type = CSData_type_Bool;
    data.var.Bool = init_value;
    return data;
}


static inline CSData_t CSData_createBuff4(const uint8_t* init_value)
{
    CSData_t data;
    data.type = CSData_type_Buff4;
    memcpy(data.var.Buff, init_value, sizeof(uint8_t) * 4);
    return data;
}

static inline CSData_t CSData_createBuff3(const uint8_t* init_value)
{
    CSData_t data;
    data.type = CSData_type_Buff3;
    memcpy(data.var.Buff, init_value, sizeof(uint8_t) * 3);
    return data;
}

static inline CSData_t CSData_createBuff2(const uint8_t* init_value)
{
    CSData_t data;
    data.type = CSData_type_Buff2;
    memcpy(data.var.Buff, init_value, sizeof(uint8_t) * 2);
    return data;
}

static inline CSData_t CSData_createBuff1(const uint8_t* init_value)
{
    CSData_t data;
    data.type = CSData_type_Buff1;
    memcpy(data.var.Buff, init_value, sizeof(uint8_t) * 1);
    return data;
}

static inline CSData_t CSData_createNull(void)
{
    CSData_t data;
    data.type = CSData_type_Null;
    return data;
}

#ifdef __cplusplus
}
#endif


#endif /*CAN_SMBUS_SRC_CORE_CS_DATA_H*/