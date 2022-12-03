#include "sa_file.h"

#include <sys/types.h>

#include "system/sa_sys_log.h"


FILE* SAFile_fopen(const char *pathname, const char *mode)
{
    FILE* fp = fopen(pathname, mode);
    if(fp == NULL)
    {
        SASYS_LOG_ERROR_ERRNO("fclose failed!", 0);
        return NULL;
    }
    return fp;
}

void SAFile_fclose(FILE *stream)
{
    int res = fclose(stream);
    if(res != 0)
    {
        SASYS_LOG_ERROR_ERRNO("fclose failed!", res);
    }
}

void SAFile_setlinebuf(FILE* stream)
{
    setlinebuf(stream);
}

void SAFile_mkdir(const char *pathname, mode_t mode)
{
    int res = mkdir(pathname, mode);
    if(res != 0)
    {
        char buff[128];
        sprintf(buff, "mkdir failed! (%s)", pathname);
        SASYS_LOG_ERROR_ERRNO(buff, res);
    }
}