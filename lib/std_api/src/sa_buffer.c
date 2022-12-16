#include "sa_buffer.h"

#include "system/sa_sys_log.h"
#include "system/sa_sys_mem.h"

#define CC_AUTO_BUFFER_ONE_BLOCK_SIZE   (1024)

void SABuffer_realloc(SABuffer_t* obj)
{
    size_t befo_block_count = obj->block_count;
    size_t allocating_count = obj->one_block_count * obj->block_count;
    
    if(allocating_count < obj->data_count)
    {
        size_t append_block_count = ((obj->data_count - allocating_count) / obj->one_block_count) + 1;
        obj->block_count += append_block_count;
    }else{
        size_t detach_block_count = ((allocating_count - obj->data_count) / obj->one_block_count);
        obj->block_count -= detach_block_count;
        if(obj->block_count <= 0)
        {
            obj->block_count = 1;
        }
    }
    
    if(befo_block_count != obj->block_count)
    {
        obj->buffer = (uint8_t*)SASYS_MEM_REALLOC(obj->buffer, obj->one_data_size * obj->one_block_count * obj->block_count);
    }
}


void SABuffer_create(SABuffer_t* obj, size_t one_data_size)
{
    obj->one_data_size = one_data_size;
    obj->data_count = 0;
    obj->block_count = 1;
    obj->one_block_count = CC_AUTO_BUFFER_ONE_BLOCK_SIZE / one_data_size;

    obj->buffer = (uint8_t*)SASYS_MEM_MALLOC(obj->one_data_size * obj->one_block_count * obj->block_count);
}


void SABuffer_destructor(SABuffer_t* obj)
{
    SASYS_MEM_FREE(obj->buffer);
}

void SABuffer_clear(SABuffer_t* obj)
{
    obj->data_count = 0;
}

void SABuffer_clone(const SABuffer_t* obj, SABuffer_t* out_obj)
{
    out_obj->one_data_size = obj->one_data_size;
    out_obj->data_count = obj->data_count;
    out_obj->block_count = obj->block_count;
    out_obj->one_block_count = obj->one_block_count;

    out_obj->buffer = (uint8_t*)SASYS_MEM_MALLOC(obj->one_data_size * obj->one_block_count * obj->block_count);
    SABuffer_add_stream(out_obj, SABuffer_count(obj), SABuffer_readAtIndex_pointer(obj, 0));
}


const void* SABuffer_getRaw(SABuffer_t* obj)
{
    return (const void*)obj->buffer;
}

size_t SABuffer_count(SABuffer_t* obj)
{
    return obj->data_count;
}


void SABuffer_add(SABuffer_t* obj, const void* data_in)
{
    SABuffer_add_stream(obj, 1, data_in);
}

void SABuffer_add_stream(SABuffer_t* obj, size_t data_count, const void* data_in)
{
    SABuffer_insert_stream(obj, obj->data_count, data_count, data_in);
}

void SABuffer_insert(SABuffer_t* obj, size_t index, const void* data_in)
{
    SABuffer_insert_stream(obj, index, 1, data_in);
}

void SABuffer_insert_stream(SABuffer_t* obj, size_t index, size_t data_count, const void* data_in)
{
    if(obj->data_count < index)
    {
        SASYS_LOG_ERROR("Index out of range!");
        return;
    }
    size_t befo_count = obj->data_count;
    obj->data_count += data_count;
    SABuffer_realloc(obj);

    memmove(&obj->buffer[obj->one_data_size * (index + data_count)], &obj->buffer[obj->one_data_size * index], obj->one_data_size * (befo_count - index));
    memcpy(&obj->buffer[obj->one_data_size * index], data_in, obj->one_data_size * data_count);
}

void SABuffer_replace(SABuffer_t* obj, size_t index, const void* data_in)
{
    SABuffer_replace_stream(obj, index, 1, data_in);
}

void SABuffer_replace_stream(SABuffer_t* obj, size_t index, size_t data_count, const void* data_in)
{
    if(obj->data_count < index + data_count)
    {
        SASYS_LOG_ERROR("Index and data_count out of range!");
        return;
    }
    memcpy(&obj->buffer[obj->one_data_size * index], data_in, obj->one_data_size * data_count);
}

void* SABuffer_readLast_pointer(SABuffer_t* obj)
{
    if(obj->data_count == 0)
    {
        SASYS_LOG_ERROR("This buffer is empty.");
        return NULL;
    }

    return SABuffer_readAtIndex_pointer(obj, obj->data_count - 1);
}

void* SABuffer_readAtIndex_pointer(SABuffer_t* obj, size_t index)
{
    if(obj->data_count <= index)
    {
        SASYS_LOG_ERROR("index out of range.");
        return NULL;
    }
    return &obj->buffer[obj->one_data_size * index];
}

void SABuffer_readLast(SABuffer_t* obj, void* data_out)
{
    if(obj->data_count == 0)
    {
        SASYS_LOG_ERROR("This buffer is empty.");
        data_out = NULL;
        return;
    }
    SABuffer_read_stream(obj, obj->data_count - 1, 1, data_out);
}

void SABuffer_readAtIndex(SABuffer_t* obj, size_t index, void* data_out)
{
    SABuffer_read_stream(obj, index, 1, data_out);
}

void SABuffer_read_stream(SABuffer_t* obj, size_t index, size_t data_count, void* data_out)
{
    if(obj->data_count < index + data_count)
    {
        SASYS_LOG_ERROR("index out of range.");
        data_out = NULL;
        return;
    }

    memcpy(data_out, &obj->buffer[obj->one_data_size * index], obj->one_data_size * data_count);
}


void SABuffer_removeLast(SABuffer_t* obj)
{
    if(obj->data_count == 0)
    {
        SASYS_LOG_ERROR("This buffer is empty.");
        return;
    }

    SABuffer_removeAtIndex(obj, obj->data_count - 1);
}

void SABuffer_removeAtIndex(SABuffer_t* obj, size_t index)
{
    SABuffer_remove_stream(obj, index, 1);
}

void SABuffer_removeAll(SABuffer_t* obj)
{
    SABuffer_remove_stream(obj, 0, obj->data_count);
}

void SABuffer_remove_stream(SABuffer_t* obj, size_t index, size_t data_count)
{
    if(obj->data_count < index + data_count)
    {
        SASYS_LOG_ERROR("index out of range.");
        return;
    }

    size_t befo_count = obj->data_count;
    obj->data_count -= data_count;

    memmove(&obj->buffer[obj->one_data_size * index], &obj->buffer[obj->one_data_size * (index + data_count)], obj->one_data_size * (befo_count - (index + data_count)));

    SABuffer_realloc(obj);
}


void SABuffer_sort(SABuffer_t* obj, SABuffer_sortCmp_callback_t cmp_callback)
{
    qsort(obj->buffer, obj->data_count, obj->one_data_size, cmp_callback);
}