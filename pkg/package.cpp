#include "package.hpp"

void PKGInitializer_init(SABool_t is_can_smbus, size_t gw_count)
{
    if(is_can_smbus)
    {
        CSInitializer_init();
        RMInitializer_init(gw_count);
        controller::daemon::init();
    }
}