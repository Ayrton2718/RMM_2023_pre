#ifndef STD_API_SRC_SA_MEM_H
#define STD_API_SRC_SA_MEM_H

#include "system/sa_type.h"

#ifdef __cplusplus
extern "C" {
#endif

void SAMem_init(void);

void* SAMem_malloc(const char* file, size_t line, size_t size);
void* SAMem_realloc(const char* file, size_t line, void* ptr, size_t size);
void SAMem_free(const char* file, size_t line, void* ptr);


#ifdef SATYPE_TRACE_ON
    #define SAMEM_MALLOC(size)          SAMem_malloc(__FILE__, __LINE__, size)
    #define SAMEM_REALLOC(ptr, size)    SAMem_realloc(__FILE__, __LINE__, ptr, size)
    #define SAMEM_FREE(ptr)             SAMem_free(__FILE__, __LINE__, ptr)
#else
    #define SAMEM_MALLOC(size)          malloc(size)
    #define SAMEM_REALLOC(ptr, size)    realloc(ptr, size)
    #define SAMEM_FREE(ptr)             free(ptr)
#endif /*CSMEM_DEBUG_ON*/

#ifdef __cplusplus
}
#endif

#endif /*STD_API_SRC_SA_MEM_H*/