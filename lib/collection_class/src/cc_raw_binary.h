#ifndef CC_RAW_BINARY_H
#define CC_RAW_BINARY_H

#include "cc_type.h"

#include "cc_base_object.h"

#ifdef __cplusplus
extern "C" {
#endif

// typedef void (*CCRawBinary_jsonMaker_t)(const void* data, size_t data_size, SABuffer_t* string, SABool_t visible);
// typedef void (*CCRawBinary_jsonLoader_t)(const void* data, size_t data_size, struct CCJsonParser_t* parser, struct CCJsonParser_jsonToken_t* json_token);

// CC_obj CCRawBinary_create(size_t data_size, CCRawBinary_jsonMaker_t json_maker, CCRawBinary_jsonLoader_t json_loader, CC_type_t type_id);

// void CCRawBinary_set(CC_obj obj, const void* data_in, CC_type_t type_id);
// void CCRawBinary_setAtIndex(CC_obj obj, size_t index, const void* data_in, CC_type_t type_id);

// void CCRawBinary_get(CC_obj obj, void* data_out, CC_type_t type_id);

// size_t CCRawBinary_dataSize(CC_obj obj, CC_type_t type_id);


struct CCRawBinary_t
{
    CCBaseObject_t object;

    size_t data_size;
    void* data_bin;
};


CC_obj CCRawBinary_create(size_t data_size, CC_type_t type_id, CCBaseObject_dump_callback_nolock_t dumper, CCBaseObject_load_callback_nolock_t loader);

#ifdef __cplusplus
}
#endif

#endif /*CC_RAW_BINARY_H*/
