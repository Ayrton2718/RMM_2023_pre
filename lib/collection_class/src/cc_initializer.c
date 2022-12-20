#include "cc_initializer.h"

#include "cc_auto_release.h"
#include "cc_dead_queue.h"

void CCInitializer_init(void)
{
    CCDeadQueue_initializer();
    CCAutoRelease_ownerInit();
}
