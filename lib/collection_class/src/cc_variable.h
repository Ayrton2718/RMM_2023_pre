// #ifndef CC_VARIABLE_H
// #define CC_VARIABLE_H

// #include "cc_type.h"

// #ifdef __cplusplus
// extern "C" {
// #endif

// typedef enum
// {
// 	CCVariable_data_Int64 = 0,
// 	CCVariable_data_Uint64,
// 	CCVariable_data_Int32,
// 	CCVariable_data_Uint32,
// 	CCVariable_data_Int16,
// 	CCVariable_data_Uint16,
//     CCVariable_data_Int8,
// 	CCVariable_data_Uint8,

// 	CCVariable_data_Float,
// 	CCVariable_data_Double,

//     CCVariable_data_Buff1,
//     CCVariable_data_Buff2,
//     CCVariable_data_Buff3,
//     CCVariable_data_Buff4
// } CCVariable_data_t;

// const char* CCVariable_dataTypeString(CCVariable_data_t data_type);


// typedef union
// {
//     int64_t Int64;
//     uint64_t Uint64;

//     int32_t Int32;
//     uint32_t Uint32;

//     int16_t Int16;
//     uint16_t Uint16;

//     int8_t Int8;
//     uint8_t Uint8;

//     float Float;
//     double Double;

//     uint8_t buff[4];
// } CCVariable_t;


// CC_obj CCVariable_create(CCVariable_t init_value, CCVariable_data_t data_type);

// void CCVariable_set(CC_obj obj, CCVariable_t set_value);
// void CCVariable_setBin(CC_obj obj, const void* binary);

// CCVariable_t CCVariable_get(CC_obj obj);

// size_t CCVariable_dataSize(CC_obj obj);

// size_t CCVariable_dataSizeFromType(CCVariable_data_t data_type);

// #ifdef __cplusplus
// }
// #endif

// #endif /*CC_VARIABLE_H*/
