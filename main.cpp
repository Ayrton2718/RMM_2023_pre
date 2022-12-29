#include "lib/library.hpp"
#include "pkg/package.hpp"


#include <arpa/inet.h>

int main(void)
{
    LIBInitializer_init("me", "/tmp/rmm_RobotRMM_2022_current");
    PKGInitializer_init(SABOOL_TRUE, 1);
    
    // can_smbus::connectionWait(0);
    // can_smbus::safetyOff();

    int sock = PCMdns_createSocket(PCMdnsProt_Ipv4Udp, NULL);
    PCMdnsRegister_t reg = PCMdnsRegister_create(sock, NULL, "pc_server", "MyTest2");

    // SATime_delaySec(2);

    // {
    //     PCTable_instance_t instance = PCTable_loockUp("pc_server", "MyTest");
    //     struct sockaddr_in addr_in = PCTable_getIp(instance);
    //     SALOG_INFO("main", "%s, %d", inet_ntoa(addr_in.sin_addr), ntohs(addr_in.sin_port));
    // }
    // {
    //     PCTable_instance_t instance = PCTable_loockUp("pc_server", "MyTest");
    //     struct sockaddr_in addr_in = PCTable_getIp(instance);
    //     SALOG_INFO("main", "%s, %d", inet_ntoa(addr_in.sin_addr), ntohs(addr_in.sin_port));
    // }

    // {
    //     PCTable_instance_t instance = PCTable_loockUp("pc_server", "MyTest2");
    //     struct sockaddr_in addr_in = PCTable_getIp(instance);
    //     SALOG_INFO("main", "%s, %d", inet_ntoa(addr_in.sin_addr), ntohs(addr_in.sin_port));
    // }

    // PCMdnsRegister_close(reg);

    // can_smbus::LPMotor mot(1, can_smbus::Port_1);

    // SALOG_CREATE_FILE("main");

    while(1)
    {
        {
            PCTable_instance_t instance = PCTable_loockUp("pc_server", "MyTest2");
            struct sockaddr_in addr_in = PCTable_getIp(instance);
            SALOG_INFO("main", "%s, %d", inet_ntoa(addr_in.sin_addr), ntohs(addr_in.sin_port));
        }
        // SALOG_INFO("main", "Hello");
        std_api::Delay::ms(100);
    }
}
