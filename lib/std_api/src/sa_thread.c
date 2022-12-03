#include "sa_thread.h"

#include "system/sa_sys_log.h"
#include "sa_locker.h"
#include "sa_buffer.h"
#include "sa_bool.h"


struct SAThread_t
{
    pthread_mutex_t mutex;
    SABuffer_t list;
};

static struct SAThread_t g_obj;


static SABool_t SAThread_searchList(pthread_t* thread, SAThread_threadInfo_t* out_info);

void SAThread_init(void)
{
    SALOCKER_INIT(&g_obj.mutex, NULL);

    SABuffer_create(&g_obj.list, sizeof(SAThread_threadInfo_t));
}

void SAThread_create(const char* file, size_t line, const char* tag, pthread_t* thread, pthread_attr_t* attr, void* (*start_routine)(void*), void* arg)
{
    int res = pthread_create(thread, attr, start_routine, arg);
    if(res != 0)
    {
        SASYS_LOG_ERROR_ERRNO_MANUAL(file, line, "pthread_create failed!", res);
    }

    SAThread_threadInfo_t info;
    info.thread = *thread;
    info.file = file;
    info.line = line;

    if(tag != NULL)
    {
        size_t name_len = strlen(tag);
        if(64 <= name_len)
        {
            name_len = 64 - 1;
        }
        memcpy(info.name, tag, sizeof(char) * name_len);
        info.name[name_len] = '\0';
    }else{
        memcpy(info.name, "no name", sizeof(char) * sizeof("no name"));
    }

    SALOCKER_LOCK(&g_obj.mutex);
    SABuffer_add(&g_obj.list, &info);
    SALOCKER_UNLOCK(&g_obj.mutex);
}

void SAThread_exit(void *retval)
{
    SABool_t is_hit = SABOOL_FALSE;

    pthread_t self = pthread_self();

    SALOCKER_LOCK(&g_obj.mutex);
    for(size_t i = 0; i < SABuffer_count(&g_obj.list); i++)
    {
        SAThread_threadInfo_t* cur_info = (SAThread_threadInfo_t*)SABuffer_readAtIndex_pointer(&g_obj.list, i);
        if(pthread_equal(self, cur_info->thread) != 0)
        {
            SABuffer_removeAtIndex(&g_obj.list, i);
            is_hit = SABOOL_TRUE;
            break;
        }
    }
    SALOCKER_UNLOCK(&g_obj.mutex);

    if(is_hit == SABOOL_FALSE)
    {
        SASYS_LOG_ERROR("Undefined thread!");
    }

    pthread_exit(retval);
}


SABool_t SAThread_getInfo(pthread_t thread, SAThread_threadInfo_t* info)
{
    if(SAThread_searchList(&thread, info))
    {
        return SABOOL_TRUE;
    }

    info->file = "unknown";
    info->line = 0;
    memcpy(info->name, "unknown", sizeof(char) * sizeof("unknown"));
    return SABOOL_FALSE;
}

static SABool_t SAThread_searchList(pthread_t* thread, SAThread_threadInfo_t* out_info)
{
    SABool_t is_hit = SABOOL_FALSE;

    SALOCKER_LOCK(&g_obj.mutex);
    for(size_t i = 0; i < SABuffer_count(&g_obj.list); i++)
    {
        SAThread_threadInfo_t* cur_info = (SAThread_threadInfo_t*)SABuffer_readAtIndex_pointer(&g_obj.list, i);
        if(pthread_equal(*thread, cur_info->thread) != 0)
        {
            out_info = cur_info;
            is_hit = SABOOL_TRUE;
            break;
        }
    }
    SALOCKER_UNLOCK(&g_obj.mutex);

    return is_hit;
}