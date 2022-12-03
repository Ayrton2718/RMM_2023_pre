#ifndef STD_API_SRC_SYSTEM_SA_SYS_LOG_H
#define STD_API_SRC_SYSTEM_SA_SYS_LOG_H

#include "sa_type.h"
#include <stdio.h>

#ifdef __cplusplus
 extern "C" {
#endif

void SASysLog_initializer(void);

void SASysLog_setOut(FILE* fp);

void SASysLog_withFormat(const char* file, size_t line, const char* level, const char* fmt, ...);

void SASysLog_withErrno(const char* file, size_t line, const char* str, int res);

#define SASYS_LOG_INFO(...)                      SASysLog_withFormat(__FILE__, __LINE__, "INFO", __VA_ARGS__)
#define SASYS_LOG_ERROR(...)                     SASysLog_withFormat(__FILE__, __LINE__, "ERROR", __VA_ARGS__)

#define SASYS_LOG_ERROR_MANUAL(file, line, ...)  SASysLog_withFormat(file, line, "ERROR", __VA_ARGS__)

#define SASYS_LOG_ERROR_ERRNO(str, res)                      SASysLog_withErrno(__FILE__, __LINE__, str, res)
#define SASYS_LOG_ERROR_ERRNO_MANUAL(file, line, str, res)   SASysLog_withErrno(file, line, str, res)

#ifdef __cplusplus
}
#endif

#endif /* STD_API_SRC_SYSTEM_SA_SYS_LOG_H */