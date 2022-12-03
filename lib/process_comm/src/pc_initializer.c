#include "pc_initializer.h"

#include "pc_type.h"
#include "pc_client.h"
#include "pc_server.h"
#include "pc_mdns.h"

#include <limits.h>
#include <unistd.h>
#include <errno.h>


#ifndef HOST_NAME_MAX
    #define HOST_NAME_MAX   (255)
#endif /*HOST_NAME_MAX*/


void PCInitializer_init(const char* server_name)
{
    // TCDirectory_mkdir("/", "process_comm");
    // TCDirectory_mkdir("process_comm", "my_servers");
    // TCDirectory_mkobj("process_comm/my_servers", "name", CCString_create(server_name));

    // PCServer_init();
    // PCClient_init();
    // PCClient_init();
}
