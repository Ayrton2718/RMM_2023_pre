#include "sa_log.h"

#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>

#include "system/sa_sys_log.h"
#include "sa_mem.h"
#include "sa_file.h"
#include "sa_time.h"
#include "sa_buffer.h"
#include "sa_locker.h"

#define SALOG_HASH_TABLE (16)


typedef struct
{
    char tag[64];
    size_t tag_len;
    FILE* fp;
} SALog_tagInfo_t;


typedef struct
{
    SABool_t is_enable;
    pthread_mutex_t locker;
    char log_dir[256];
    SABuffer_t tag_info[SALOG_HASH_TABLE];
} SALog_info_t;

static SALog_info_t g_obj;


static uint8_t SALog_createHash(const char* tag, size_t tag_len);
static FILE* SALog_searchTagInfo(const char* tag, size_t tag_len);


void SALog_init(const char* dir)
{
    SALOCKER_INIT(&g_obj.locker, NULL);

    struct stat statBuf;
    if(dir == NULL || (stat(dir, &statBuf) != 0))
    {
        g_obj.is_enable = SABOOL_FALSE;

        struct timeval myTime;
        struct tm time_st;
        gettimeofday(&myTime, NULL);
        memcpy(&time_st, localtime(&myTime.tv_sec), sizeof(struct tm));

        sprintf(g_obj.log_dir, "/tmp/%d_%02d_%02d-%02d_%02d_%02d",
                    time_st.tm_year+1900,
                    time_st.tm_mon+1,
                    time_st.tm_mday,
                    time_st.tm_hour,
                    time_st.tm_min,
                    time_st.tm_sec);

        SAFile_mkdir(g_obj.log_dir,
                    S_IRUSR | S_IWUSR | S_IXUSR |           //ユーザーの権限（読み込み、書き込み、実行）
                    S_IRGRP | S_IWGRP | S_IXGRP |           //グループの権限（読み込み、書き込み、実行）
                    S_IROTH | S_IWOTH | S_IXOTH);           //その他の権限（読み込み、書き込み、実行）
    }
    else
    {
        g_obj.is_enable = SABOOL_TRUE;
        sprintf(g_obj.log_dir, "%s", dir);
    }

    for(size_t i = 0; i < SALOG_HASH_TABLE; i++)
    {
        SABuffer_create(&g_obj.tag_info[i], sizeof(SALog_tagInfo_t));
    }

    SALog_createFile("errors", sizeof("errors") - 1);
    FILE* fp = SALog_searchTagInfo("errors", sizeof("errors") - 1);
    if(fp != NULL)
    {
        SASysLog_setOut(fp);
    }
}


void SALog_createFile(const char* tag, size_t tag_len)
{
    char string_info[256];
    char string_error[256];
    sprintf(string_info, "%s/%s.txt", g_obj.log_dir, tag);
    FILE* fp = SAFile_fopen(string_info, "w");
    if(fp == NULL)
    {
        return;
    }

    SAFile_setlinebuf(fp);

    uint8_t hash = SALog_createHash(tag, tag_len);
    SABool_t is_hit = SABOOL_FALSE;

    SALOCKER_LOCK(&g_obj.locker);
    size_t buff_count = SABuffer_count(&g_obj.tag_info[hash]);
    SALog_tagInfo_t* tags = (SALog_tagInfo_t*)SABuffer_getRaw(&g_obj.tag_info[hash]);
    for(size_t i = 0; i < buff_count; i++)
    {
        if(tags[i].tag_len == tag_len)
        {
            if(memcmp(tags[i].tag, tag, sizeof(char) * tag_len) == 0)
            {
                tags[i].fp = fp;
                is_hit = SABOOL_TRUE;
                break;
            }
        }
    }
    
    if(is_hit == SABOOL_FALSE)
    {
        SALog_tagInfo_t tag_info;

        tag_info.tag_len = tag_len;
        memcpy(tag_info.tag, tag, tag_len);
        tag_info.tag[tag_len] = '\0';

        tag_info.fp = fp;

        SABuffer_add(&g_obj.tag_info[hash], &tag_info);
    }
    SALOCKER_UNLOCK(&g_obj.locker);
}


void SALog_disableTag(const char* tag, size_t tag_len)
{
    uint8_t hash = SALog_createHash(tag, tag_len);
    SABool_t is_hit = SABOOL_FALSE;

    SALOCKER_LOCK(&g_obj.locker);
    size_t buff_count = SABuffer_count(&g_obj.tag_info[hash]);
    SALog_tagInfo_t* tags = (SALog_tagInfo_t*)SABuffer_getRaw(&g_obj.tag_info[hash]);
    for(size_t i = 0; i < buff_count; i++)
    {
        if(tags[i].tag_len == tag_len)
        {
            if(memcmp(tags[i].tag, tag, sizeof(char) * tag_len) == 0)
            {
                tags[i].fp = NULL;
                is_hit = SABOOL_TRUE;
                break;
            }
        }
    }
    
    if(is_hit == SABOOL_FALSE)
    {
        SALog_tagInfo_t tag_info;

        tag_info.tag_len = tag_len;
        memcpy(tag_info.tag, tag, tag_len);
        tag_info.tag[tag_len] = '\0';

        tag_info.fp = NULL;

        SABuffer_add(&g_obj.tag_info[hash], &tag_info);
    }
    SALOCKER_UNLOCK(&g_obj.locker);
}


void SALog_withFormat(const char* tag, size_t tag_len, SALog_logLevel_t level, const char* file, size_t line, const char* fmt, ...)
{
    char str[1024];
    int len;
    va_list ap;
    va_list ap_copy;

    va_copy(ap_copy, ap);
    
    va_start(ap, fmt);
    len = vsnprintf(str, 1024, fmt, ap);
    va_end(ap);
    if(len < 0)
    {
        SASYS_LOG_ERROR_ERRNO("vsnprintf Error", len);
        return;
    }
    
    if(len <= 1024)
    {
        SALog_noFormat(tag, tag_len, level, file, line, str);
    }
    else
    {
        char* tmp_buff = (char*)SAMEM_MALLOC(sizeof(char) * (len + 1));

        va_start(ap_copy, fmt);
        len = vsnprintf(tmp_buff, len+1, fmt, ap_copy); 
        va_end(ap_copy);

        if(len < 0)
        {
            SASYS_LOG_ERROR_ERRNO("vsnprintf Error", len);
            SAMEM_FREE(tmp_buff);
            return;
        }

        SALog_noFormat(tag, tag_len, level, file, line, tmp_buff);
        SAMEM_FREE(tmp_buff);
    }
}

void SALog_noFormat(const char* tag, size_t tag_len, SALog_logLevel_t level, const char* file, size_t line, const char* str)
{
    struct timeval myTime;
    struct tm time_st;

    gettimeofday(&myTime, NULL);
    memcpy(&time_st, localtime(&myTime.tv_sec), sizeof(struct tm));
    
    if((level == SALog_logLevel_Info) && g_obj.is_enable)
    {
        FILE* fp = SALog_searchTagInfo(tag, tag_len);
        if(fp != NULL)
        {
            fprintf(fp, "[%6s] [%02d:%02d:%02d.%06ld] [%s:%ld, %s]: %s\n",
                    "INFO",
                    time_st.tm_hour,
                    time_st.tm_min,
                    time_st.tm_sec,
                    (long)myTime.tv_usec,
                    file,
                    line,
                    tag,
                    str);
        }
    }
    else if((level == SALog_logLevel_Warn) && g_obj.is_enable)
    {
        FILE* fp = SALog_searchTagInfo(tag, tag_len);
        if(fp != NULL)
        {
            fprintf(fp, "[%6s] [%02d:%02d:%02d.%06ld] [%s:%ld, %s]: %s\n",
                    "WARN",
                    time_st.tm_hour,
                    time_st.tm_min,
                    time_st.tm_sec,
                    (long)myTime.tv_usec,
                    file,
                    line,
                    tag,
                    str);
        }
    }
    else if(level == SALog_logLevel_Error)
    {
        FILE* fp = SALog_searchTagInfo(tag, tag_len);
        if(fp != NULL)
        {
            fprintf(fp, "[%6s] [%02d:%02d:%02d.%06ld] [%s:%ld, %s]: %s\n",
                "ERROR",
                time_st.tm_hour,
                time_st.tm_min,
                time_st.tm_sec,
                (long)myTime.tv_usec,
                file,
                line,
                tag,
                str);
        }

        FILE* err_fp = SALog_searchTagInfo("errors", sizeof("errors") - 1);
        if(err_fp != NULL)
        {
            fprintf(err_fp, "[%6s] [%02d:%02d:%02d.%06ld] [%s:%ld, %s]: %s\n",
                "ERROR",
                time_st.tm_hour,
                time_st.tm_min,
                time_st.tm_sec,
                (long)myTime.tv_usec,
                file,
                line,
                tag,
                str);
        }
    }
}


static uint8_t SALog_createHash(const char* tag, size_t tag_len)
{
    uint8_t hash = 0;
    for(size_t i = 0; i < tag_len; i++)
    {
        hash += (uint8_t)tag[i];
    }
    return hash % SALOG_HASH_TABLE;
}


static FILE* SALog_searchTagInfo(const char* tag, size_t tag_len)
{
    FILE* res;
    
    uint8_t hash = SALog_createHash(tag, tag_len);
    SABool_t is_hit = SABOOL_FALSE;

    SALOCKER_LOCK(&g_obj.locker);
    size_t buff_count = SABuffer_count(&g_obj.tag_info[hash]);
    SALog_tagInfo_t* tags = (SALog_tagInfo_t*)SABuffer_getRaw(&g_obj.tag_info[hash]);
    for(size_t i = 0; i < buff_count; i++)
    {
        if(tags[i].tag_len == tag_len)
        {
            if(memcmp(tags[i].tag, tag, sizeof(char) * tag_len) == 0)
            {
                res = tags[i].fp;
                is_hit = SABOOL_TRUE;
                break;
            }
        }
    }
    
    if(is_hit == SABOOL_FALSE)
    {
        SALog_tagInfo_t tag_info;

        tag_info.tag_len = tag_len;
        memcpy(tag_info.tag, tag, tag_len);
        tag_info.tag[tag_len] = '\0';

        tag_info.fp = stdout;

        SABuffer_add(&g_obj.tag_info[hash], &tag_info);

        res = tag_info.fp;
    }
    SALOCKER_UNLOCK(&g_obj.locker);

    return res;
}
