#include "sa_sys_mem.h"

#include "sa_sys_log.h"


void* SASysMem_malloc(const char* file, size_t line, size_t size)
{
    void* ptr = malloc(size);
    if(ptr == NULL)
    {
        SASYS_LOG_ERROR_ERRNO_MANUAL(file, line, "Failed malloc!", 0);
    }
    return ptr;
}


void* SASysMem_realloc(const char* file, size_t line, void* ptr, size_t size)
{
    void* new_ptr = realloc(ptr, size);
    if(new_ptr == NULL)
    {
        SASYS_LOG_ERROR_ERRNO_MANUAL(file, line, "Failed realloc!", 0);
    }
    return new_ptr;
}


// void SASysMem_free(const char* file, size_t len, void* ptr)
// {
//     return free(ptr);
// }