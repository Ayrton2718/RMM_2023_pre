#include "pc_initializer.h"

#include "pc_type.h"
#include "pc_client.h"
#include "pc_server.h"
#include "pc_mdns.h"
#include "pc_table.h"

#include <limits.h>
#include <unistd.h>
#include <errno.h>


#ifndef HOST_NAME_MAX
    #define HOST_NAME_MAX   (255)
#endif /*HOST_NAME_MAX*/


void PCInitializer_init(const char* server_name)
{
    PCTable_init();

    PCMdns_init();

    // PCServer_init(server_name);
    // PCClient_init();
}
