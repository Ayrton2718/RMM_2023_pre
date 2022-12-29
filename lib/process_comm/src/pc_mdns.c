#include "pc_mdns.h"

#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#ifdef __APPLE__
#include "pc_dnssd.h"
#elif __linux__
#include "pc_avahi.h"
#endif

void PCMdns_init(void)
{
    PCDnssd_init();
}


int PCMdns_createSocket(PCMdnsProt_t protocol, struct sockaddr_in* addr_out)
{
    int sock;
    switch(protocol)
    {
    case PCMdnsProt_Ipv4Udp:
        sock = SASocket_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;

    case PCMdnsProt_Ipv6Udp:
        sock = SASocket_socket(AF_INET6, SOCK_STREAM, IPPROTO_UDP);
        break;

    case PCMdnsProt_Ipv4Tcp:
        sock = SASocket_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;

    case PCMdnsProt_Ipv6Tcp:
        sock = SASocket_socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
        break;
    
    default:
        SALOG_ERROR("process_comm", "Undefined protocol, %d", protocol);
        return -1;
    }

    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(0);

    SASocket_bind(sock, (const struct sockaddr*)&addr, addr_len);

    struct sockaddr_in binded_addr;
	if(getsockname(sock, (struct sockaddr*)&binded_addr, &addr_len) != -1)
	{
        SALOG_INFO("process_comm", "Binded at %d", ntohs(binded_addr.sin_port));
        if(addr_out != NULL)
        {
            memcpy(addr_out, &binded_addr, addr_len);
        }
        return sock;
	}else{
        SALOG_ERROR("process_comm", "Failed getsockname,");
    }

    return -1;

}


PCMdnsRegister_t PCMdnsRegister_create(int sock, const struct sockaddr_in* addr_in, const char* service_name, const char* name)
{
    return PCDnssd_registServer(sock, addr_in, service_name, name);
}

void PCMdnsRegister_close(PCMdnsRegister_t service)
{
    PCDnssd_closeServer(service);
}


// PCTable_info_t PCMdnsServer_browse(const char* service_name, const char* name)
// {

// }

// void PCMdnsServer_sendto(int sock, PCTable_info_t server, const void *buf, size_t len)
// {

// }









// int PCMdns_createSocket(PCMdnsProt_t protocol, struct sockaddr_in* addr_out)
// {
//     int sock;
//     switch(protocol)
//     {
//     case PCMdnsProt_Ipv4Udp:
//         sock = SASOCKET_SOCKET(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//         break;

//     case PCMdnsProt_Ipv6Udp:
//         sock = SASOCKET_SOCKET(AF_INET6, SOCK_STREAM, IPPROTO_UDP);
//         break;

//     case PCMdnsProt_Ipv4Tcp:
//         sock = SASOCKET_SOCKET(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//         break;

//     case PCMdnsProt_Ipv6Tcp:
//         sock = SASOCKET_SOCKET(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
//         break;
    
//     default:
//         SALOG_ERROR("process_comm", "Undefined protocol, %d", protocol);
//         return -1;
//     }

//     struct sockaddr_in addr;
//     socklen_t addr_len = sizeof(struct sockaddr_in);
//     addr.sin_family = AF_INET;
//     addr.sin_addr.s_addr = INADDR_ANY;
//     addr.sin_port = htons(0);

//     SASOCKET_BIND(sock, (const struct sockaddr*)&addr, addr_len);

//     struct sockaddr_in binded_addr;
// 	if(getsockname(sock, (struct sockaddr*)&binded_addr, &addr_len) != -1)
// 	{
//         SALOG_INFO("process_comm", "Binded at %d", ntohs(binded_addr.sin_port));
//         if(addr_out != NULL)
//         {
//             memcpy(addr_out, &binded_addr, addr_len);
//         }
//         return sock;
// 	}else{
//         SALOG_ERROR("process_comm", "Failed getsockname,");
//     }

//     return -1;
// }


// PCMdns_register_t PCMdns_registServer(int sock, const struct sockaddr_in* addr_in, const char* service_name, const char* name, PCMdns_param_t param)
// {
// // #ifdef __APPLE__
// //     return PCDnssd_registServer(sock, addr_in, service_name, name, param);
// // #elif __linux__
// //     return PCAvahi_registServer(sock, addr_in, service_name, name, param);
// // #endif /*__linux__*/
// }

// void PCMdns_closeServer(PCMdns_register_t service)
// {
// // #ifdef __APPLE__
// //     PCDnssd_closeServer(service);
// // #elif __linux__
// //     PCAvahi_closeServer(service);
// // #endif /*__linux__*/
// }


// PCMdns_server_t PCMdns_browseServer(const char* service_name, const char* name)
// {
// // #ifdef __APPLE__
// //     return PCDnssd_browseServer(service_name, name);
// // #elif __linux__
// //     return PCAvahi_browseServer(service_name, name);
// // #endif /*__linux__*/
// }


// void PCMdns_sendtoServer(int sock, PCMdns_server_t server, const void *buf, size_t len)
// {
//     // SASOCKET_SENDTO(sock, buf, len, 0, (struct sockaddr*)&server.addr, sizeof(struct sockaddr_in));
// }


// void PCMdns_printServer(PCMdns_server_t server)
// {
//     // SALOG_INFO("process_comm", "server ip=%s port=%d", inet_ntoa(server.addr.sin_addr), ntohs(server.addr.sin_port));
// }


// PCMdns_param_t PCMdns_createParam(void)
// {

// }

// void PCMdns_addParam(PCMdns_param_t param, const char* key, const char* value)
// {

// }
