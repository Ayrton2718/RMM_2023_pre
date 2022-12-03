#include "library.hpp"

void LIBInitializer_init(const char* server_name, const char* log_path)
{
    SAInitializer_init(log_path);
    CCInitializer_init();
    TCInitializer_init();
    PCInitializer_init(server_name);
}