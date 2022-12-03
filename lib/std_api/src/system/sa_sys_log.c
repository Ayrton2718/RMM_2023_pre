#include "sa_sys_log.h"

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

static FILE* g_stdout = NULL;

static void SASysLog_noFormat(FILE* out, const char* file, size_t line, const char* level, const char* str);


void SASysLog_initializer(void)
{
    SASysLog_setOut(stdout);
}


void SASysLog_setOut(FILE* fp)
{
    g_stdout = fp;
    setlinebuf(g_stdout);
}

void SASysLog_withFormatInfo(const char* file, size_t line, const char* fmt, ...)
{
    char debug_string[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(debug_string, 1023, fmt, ap);
    debug_string[1023] = 0x00;
    va_end(ap);

    SASysLog_noFormat(g_stdout, file, line, "INFO", debug_string);
}

void SASysLog_withFormat(const char* file, size_t line, const char* level, const char* fmt, ...)
{
    char debug_string[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(debug_string, 1023, fmt, ap);
    debug_string[1023] = 0x00;
    va_end(ap);

    SASysLog_noFormat(g_stdout, file, line, level, debug_string);
}


void SASysLog_withErrno(const char* file, size_t line, const char* str, int res)
{
    char debug_string[1024];
    sprintf(debug_string, "%s  <-\"%s\"(%d)", str, strerror(errno), res);
    SASysLog_noFormat(g_stdout, file, line, "ERROR", debug_string);
}


static void SASysLog_noFormat(FILE* out, const char* file, size_t line, const char* level, const char* str)
{
    fprintf(out, "[%6s] [%s:%zu]: %s\n", level, file, line, str);
}
