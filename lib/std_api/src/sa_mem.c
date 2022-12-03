#include "sa_mem.h"

#include "system/sa_sys_log.h"
#include "system/sa_sys_mem.h"
#include "sa_locker.h"
#include "sa_buffer.h"
#include "sa_bool.h"


struct SAMem_t
{
    pthread_mutex_t mutex;
    SABuffer_t list;
};

struct SAMem_memInfo_t
{
    void* ptr;
    const char* file;
    size_t line;
};

static struct SAMem_t g_obj;


void SAMem_init(void)
{
    SALOCKER_INIT(&g_obj.mutex, NULL);
    SABuffer_create(&g_obj.list, sizeof(struct SAMem_memInfo_t));
}


void* SAMem_malloc(const char* file, size_t line, size_t size)
{
    struct SAMem_memInfo_t info;
    info.ptr = SASYS_MEM_MALLOC_MANUAL(file, line, size);
    info.file = file;
    info.line = line;

    SALOCKER_LOCK(&g_obj.mutex);
    SABuffer_add(&g_obj.list, &info);
    SALOCKER_UNLOCK(&g_obj.mutex);

    return info.ptr;
}

void* SAMem_realloc(const char* file, size_t line, void* ptr, size_t size)
{
    struct SAMem_memInfo_t info;
    info.ptr = SASYS_MEM_REALLOC_MANUAL(file, line, ptr, size);

    SABool_t is_hit = SABOOL_FALSE;

    SALOCKER_LOCK(&g_obj.mutex);
    size_t buff_count = SABuffer_count(&g_obj.list);
    for(size_t i = 0; i < buff_count; i++)
    {
        struct SAMem_memInfo_t* cur_info = (struct SAMem_memInfo_t*)SABuffer_readAtIndex_pointer(&g_obj.list, i);
        if(cur_info->ptr == ptr)
        {
            info.file = cur_info->file;
            info.line = cur_info->line;
            SABuffer_replace(&g_obj.list, i, &info);
            is_hit = SABOOL_TRUE;
            break;
        }
    }
    SALOCKER_UNLOCK(&g_obj.mutex);

    if(is_hit == SABOOL_FALSE)
    {
        SASYS_LOG_ERROR("Undifined memory (%p)", ptr);
    }

    return info.ptr;
}

void SAMem_free(const char* file, size_t line, void* ptr)
{
    SASYS_MEM_FREE_MANUAL(file, line, ptr);

    SABool_t is_hit = SABOOL_FALSE;

    SALOCKER_LOCK(&g_obj.mutex);
    size_t buff_count = SABuffer_count(&g_obj.list);
    for(size_t i = 0; i < buff_count; i++)
    {
        struct SAMem_memInfo_t* cur_info = (struct SAMem_memInfo_t*)SABuffer_readAtIndex_pointer(&g_obj.list, i);
        if(cur_info->ptr == ptr)
        {
            SABuffer_removeAtIndex(&g_obj.list, i);
            is_hit = SABOOL_TRUE;
            break;
        }
    }
    SALOCKER_UNLOCK(&g_obj.mutex);

    if(is_hit == SABOOL_FALSE)
    {
        SASYS_LOG_ERROR("Undifined memory (%p)", ptr);
    }
}
