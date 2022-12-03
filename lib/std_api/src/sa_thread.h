#ifndef STD_API_SRC_SA_THREAD_H
#define STD_API_SRC_SA_THREAD_H

#include "system/sa_type.h"

#include <pthread.h>
#include "sa_bool.h"

#ifdef __cplusplus
 extern "C" {
#endif



void SAThread_init(void);

void SAThread_create(const char* file, size_t line, const char* tag, pthread_t* thread, pthread_attr_t* attr, void* (*start_routine)(void*), void* arg);

void SAThread_exit(void *retval);


typedef struct
{
    pthread_t thread;
    const char* file;
    size_t line;
    char name[64];
} SAThread_threadInfo_t;

SABool_t SAThread_getInfo(pthread_t thread, SAThread_threadInfo_t* info);


#ifdef SATYPE_TRACE_ON

#define SATHREAD_CREATE(tag, thread, attr, start_routine, arg)  SAThread_create(__FILE__, __LINE__, tag, thread, attr, start_routine, arg)

#else

#define SATHREAD_CREATE(tag, thread, attr, start_routine, arg)  pthread_create( thread, attr, start_routine, arg)

#endif /*SATYPE_TRACE_ON*/


#ifdef __APPLE__
    #define SATHREAD_YIELD() pthread_yield_np()
#else
    #define SATHREAD_YIELD() sched_yield()
#endif


#ifdef __cplusplus
}
#endif

#endif /* STD_API_SRC_SA_THREAD_H */