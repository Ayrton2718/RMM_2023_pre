#ifndef STD_API_SRC_SA_LOG_H
#define STD_API_SRC_SA_LOG_H

#include "system/sa_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    SALog_logLevel_Info = 0,
    SALog_logLevel_Warn = 1,
    SALog_logLevel_Error = 2
} SALog_logLevel_t;

void SALog_init(const char* dir);

void SALog_createFile(const char* tag, size_t tag_len);
void SALog_disableTag(const char* tag, size_t tag_len);

void SALog_withFormat(const char* tag, size_t tag_len, SALog_logLevel_t level, const char* file, size_t line, const char* fmt, ...);
void SALog_noFormat(const char* tag, size_t tag_len, SALog_logLevel_t level, const char* file, size_t line, const char* str);


#ifdef SATYPE_CHECKER_ON

#define SALOG_CREATE_FILE(tag)          SALog_createFile(tag, strlen(tag))
#define SALOG_DISABLE_TAG(tag)          SALog_disableTag(tag, strlen(tag))

#define SALOG_INFO(tag, ...)            SALog_withFormat(tag, strlen(tag), SALog_logLevel_Info, __FILE__, __LINE__, __VA_ARGS__)
#define SALOG_INFO_NOFMT(tag, str)      SALog_noFormat(tag, strlen(tag), SALog_logLevel_Info, __FILE__, __LINE__, str)

#define SALOG_WARN(tag, ...)            SALog_withFormat(tag, strlen(tag), SALog_logLevel_Warn, __FILE__, __LINE__, __VA_ARGS__)
#define SALOG_WARN_NOFMT(tag, str)      SALog_noFormat(tag, strlen(tag), SALog_logLevel_Warn, __FILE__, __LINE__, str)

#define SALOG_ERROR(tag, ...)           SALog_withFormat(tag, strlen(tag), SALog_logLevel_Error, __FILE__, __LINE__, __VA_ARGS__)
#define SALOG_ERROR_NOFMT(tag, str)     SALog_noFormat(tag, strlen(tag), SALog_logLevel_Error, __FILE__, __LINE__, str)

#else

#define SALOG_CREATE(tag, file_name)   SALog_create(tag, file_name)

#define SALOG_INFO(...)     
#define SALOG_INFO_NOFMT(...)       

#define SALOG_WARN(...)     
#define SALOG_WARN_NOFMT(...)       

#define SALOG_ERROR(tag, ...)      SALog_errorWithFormat(obj, __FILE__, __LINE__, __VA_ARGS__)
#define SALOG_ERROR_NOFMT(tag, str)      SALog_errorNoFormat(obj, __FILE__, __LINE__, str)

#endif /*SATYPE_LOG_ON*/

#ifdef __cplusplus
}
#endif

#endif /*STD_API_SRC_SA_LOG_H*/