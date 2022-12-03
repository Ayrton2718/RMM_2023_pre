#include "sa_initializer.h"

#include "system/sa_sys_log.h"
#include "sa_log.h"
#include "sa_mem.h"
#include "sa_thread.h"

void SAInitializer_init(const char* log_path)
{
    SASysLog_initializer();
    SAMem_init();
    SAThread_init();

    SALog_init(log_path);
}