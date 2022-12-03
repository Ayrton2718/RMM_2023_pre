#ifndef STD_API_SRC_SYSTEM_SA_SYS_MEM_H
#define STD_API_SRC_SYSTEM_SA_SYS_MEM_H

#include "sa_type.h"

#ifdef __cplusplus
extern "C" {
#endif

void* SASysMem_malloc(const char* file, size_t line, size_t size);
void* SASysMem_realloc(const char* file, size_t line, void* ptr, size_t size);
// void SASysMem_free(const char* file, size_t line, void* ptr);


#ifdef SATYPE_CHECKER_ON
    #define SASYS_MEM_MALLOC(size)          SASysMem_malloc(__FILE__, __LINE__, size)
    #define SASYS_MEM_REALLOC(ptr, size)    SASysMem_realloc(__FILE__, __LINE__, ptr, size)
    #define SASYS_MEM_FREE(ptr)             free(ptr)

    #define SASYS_MEM_MALLOC_MANUAL(file, line, size)          SASysMem_malloc(file, line, size)
    #define SASYS_MEM_REALLOC_MANUAL(file, line, ptr, size)    SASysMem_realloc(file, line, ptr, size)
    #define SASYS_MEM_FREE_MANUAL(file, line, ptr)             free(ptr)
#else
    #define SASYS_MEM_MALLOC(size)          malloc(size)
    #define SASYS_MEM_REALLOC(ptr, size)    realloc(ptr, size)
    #define SASYS_MEM_FREE(ptr)             free(ptr)

    #define SASYS_MEM_MALLOC_MANUAL(file, line, size)          malloc(file, line, size)
    #define SASYS_MEM_REALLOC_MANUAL(file, line, ptr, size)    realloc(file, line, ptr, size)
    #define SASYS_MEM_FREE_MANUAL(file, line, ptr)             free(ptr)
#endif /*CSMEM_DEBUG_ON*/

#ifdef __cplusplus
}
#endif

#endif /*STD_API_SRC_SYSTEM_SA_SYS_MEM_H*/