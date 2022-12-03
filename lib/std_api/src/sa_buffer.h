#ifndef STD_API_SRC_SA_BUFFER_H
#define STD_API_SRC_SA_BUFFER_H

#include "system/sa_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    size_t one_data_size;
    size_t data_count;
    size_t one_block_count;
    size_t block_count;
    uint8_t* buffer;
} SABuffer_t;

void SABuffer_create(SABuffer_t* obj, size_t one_data_size);
void SABuffer_destructor(SABuffer_t* obj);

void SABuffer_clear(SABuffer_t* obj);

size_t SABuffer_count(SABuffer_t* obj);

const void* SABuffer_getRaw(SABuffer_t* obj);

// out_obj must be uncreated.
void SABuffer_clone(const SABuffer_t* obj, SABuffer_t* out_obj);

void SABuffer_add(SABuffer_t* obj, const void* data_in);
void SABuffer_add_stream(SABuffer_t* obj, size_t data_count, const void* data_in);
void SABuffer_insert(SABuffer_t* obj, size_t index, const void* data_in);
void SABuffer_insert_stream(SABuffer_t* obj, size_t index, size_t data_count, const void* data_in);
void SABuffer_replace(SABuffer_t* obj, size_t index, const void* data_in);
void SABuffer_replace_stream(SABuffer_t* obj, size_t index, size_t data_count, const void* data_in);

void SABuffer_readLast(SABuffer_t* obj, void* data_out);
void* SABuffer_readLast_pointer(SABuffer_t* obj);
void SABuffer_readAtIndex(SABuffer_t* obj, size_t index, void* data_out);
void* SABuffer_readAtIndex_pointer(SABuffer_t* obj, size_t index);
void SABuffer_read_stream(SABuffer_t* obj, size_t index, size_t data_count, void* data_out);

void SABuffer_removeLast(SABuffer_t* obj);
void SABuffer_removeAtIndex(SABuffer_t* obj, size_t index);
void SABuffer_removeAll(SABuffer_t* obj);
void SABuffer_remove_stream(SABuffer_t* obj, size_t index, size_t data_count);

#ifdef __cplusplus
}
#endif

#endif /*STD_API_SRC_SA_BUFFER_H*/
