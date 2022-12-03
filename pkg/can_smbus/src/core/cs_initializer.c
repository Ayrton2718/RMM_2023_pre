#include "cs_initializer.h"

#include "io/cs_io.h"
#include "io/cs_tx.h"
#include "io/cs_rx.h"
#include "cs_system.h"

void CSInitializer_init(void)
{
    CSIo_initializer();
    CSTx_initializer();
    CSRx_initializer();

    CSSystem_initializer();
}