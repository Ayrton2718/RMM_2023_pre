#include "pc_mdns.h"

#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
// #include <dns_sd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// typedef struct
// {
//     size_t host_len;
//     const char* host;
//     SABuffer_t server_buff;
// } PCMdns_browseArg_t;

// void PCMdns_createServiceType(char* buffer, PCMdns_protocol_t protocol, const char* service_name)
// {
//     switch(protocol)
//     {
//     case PCMdns_protocol_Udp:
//         sprintf(buffer, "_rmm-%s._udp", service_name);
//         break;

//     case PCMdns_protocol_Tcp:
//         sprintf(buffer, "_rmm-%s._tcp", service_name);
//         break;
    
//     default:
//         SALOG_ERROR("process_comm", "Undefined protocol, %d", protocol);
//         return;
        
//     }
// }


// static void PCMdns_serviceRegister_callback(DNSServiceRef s, DNSServiceFlags f, DNSServiceErrorType e, const char *name, const char *type, const char *domain, void *v);
// static void PCMdns_serviceBrowse_callback(DNSServiceRef s, DNSServiceFlags f, uint32_t i, DNSServiceErrorType e, const char *name, const char *type, const char *domain, void *context);
// static void PCMdns_serviceResolve_callback(DNSServiceRef s, DNSServiceFlags f, uint32_t i, DNSServiceErrorType e, const char *n, const char *host, uint16_t port, uint16_t tl, const unsigned char *t, void *c);
// static void PCMdns_serviceGetAddrInfo_callback(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *hostname, const struct sockaddr *address, uint32_t ttl, void *context);



int PCMdns_createSocket(PCMdns_protocol_t protocol, struct sockaddr_in* addr_out)
{
    int sock;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    switch(protocol)
    {
    case PCMdns_protocol_Udp:
        sock = SASocket_socket(AF_INET, SOCK_DGRAM, 0);
        break;

    case PCMdns_protocol_Tcp:
        sock = SASocket_socket(AF_INET, SOCK_STREAM, 0);
        break;
    
    default:
        SALOG_ERROR("process_comm", "Undefined protocol, %d", protocol);
        return -1;
    }

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


// PCMdns_register_t PCMdns_registServer(int sock, const struct sockaddr_in* addr_in, PCMdns_protocol_t protocol, const char* service_name, const char* name, CC_obj param)
// {
//     char service_type[256];
//     PCMdns_createServiceType(service_type, protocol, service_name);

//     struct sockaddr_in addr;
//     if(addr_in != NULL)
//     {
//         memcpy(&addr, addr_in, sizeof(struct sockaddr_in));
//     }
//     else
//     {
//         socklen_t addr_len = sizeof(struct sockaddr_in);
//         if(getsockname(sock, (struct sockaddr*)&addr, &addr_len) == -1)
//         {
//             SALOG_ERROR("process_comm", "Failed getsockname,");
//         }
//     }

//     DNSServiceErrorType err;
//     DNSServiceRef* sdref;
//     TXTRecordRef txt_record;

//     sdref = (DNSServiceRef*)SAMEM_MALLOC(sizeof(DNSServiceRef));
//     *sdref = 0;

//     if(CCObject_isObject(param))
//     {
        
//     }else{
//         TXTRecordCreate(&txt_record, 0, NULL);
//     }

//     err = DNSServiceRegister(sdref, 0, 0, name, service_type, NULL, NULL, addr.sin_port, TXTRecordGetLength(&txt_record), TXTRecordGetBytesPtr(&txt_record), PCMdns_serviceRegister_callback, NULL);
// 	if (err != kDNSServiceErr_NoError)
//     {
//         SALOG_ERROR("process_comm", "Failed DNSServiceRegister (%d)", err);
//         SAMEM_FREE(sdref);
// 		return NULL;
// 	}
    
//     err = DNSServiceProcessResult(*sdref);
//     if (err != kDNSServiceErr_NoError)
//     {
//         SALOG_ERROR("process_comm", "Failed DNSServiceProcessResult (%d)", err);
//         SAMEM_FREE(sdref);
// 		return NULL;
// 	}

//     TXTRecordDeallocate(&txt_record);

//     return sdref;
// }

// static void PCMdns_serviceRegister_callback(DNSServiceRef s, DNSServiceFlags f, DNSServiceErrorType e, const char *name, const char *type, const char *domain, void *v)
// {
// 	if (e != kDNSServiceErr_NoError)
//     {
//         SALOG_ERROR("process_comm", "Failed PCMdns_serviceRegister_callback (%d)", e);
// 		return;
// 	}
// }

// void PCMdns_close(PCMdns_register_t service)
// {
//     DNSServiceRef* sdref = (DNSServiceRef*)service;
//     DNSServiceRefDeallocate(*sdref);
// }


// SABool_t PCMdns_browseServer(PCMdns_protocol_t protocol, const char* service_name, const char* name, PCMdns_server_t* res)
// {
//     SABool_t is_success = SABOOL_FALSE;
//     SABuffer_t server_buff = PCMdns_browseServerAll(protocol, service_name, name);
//     if(0 < SABuffer_count(&server_buff))
//     {
//         SABuffer_readAtIndex(&server_buff, 0, res);
//         is_success = SABOOL_TRUE;
//     }

//     SABuffer_destructor(&server_buff);
//     return is_success;
// }

// static int set_nonblocking(int fd)
// {
//     int flags;
//     /* If they have O_NONBLOCK, use the Posix way to do it */
// #if defined(O_NONBLOCK)
//     /* Fixme: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5. */
//     if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
//         flags = 0;
//     return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
// #else
//     /* Otherwise, use the old way of doing it */
//     flags = 1;
//     return ioctl(fd, FIOBIO, &flags);
// #endif
// } 

// SABuffer_t PCMdns_browseServerAll(PCMdns_protocol_t protocol, const char* service_name, const char* name)
// {
// 	DNSServiceRef sdref = 0;
//     DNSServiceErrorType err;

//     char service_type[256];
//     PCMdns_createServiceType(service_type, protocol, service_name);

//     PCMdns_browseArg_t browse_arg;
//     browse_arg.host_len = strlen(name);
//     browse_arg.host = name;
//     SABuffer_create(&browse_arg.server_buff, sizeof(PCMdns_server_t));

// 	err = DNSServiceBrowse(&sdref, 0, 0, service_type, NULL, PCMdns_serviceBrowse_callback, &browse_arg);
//     if (err != kDNSServiceErr_NoError)
//     {
//         SALOG_ERROR("process_comm", "Failed DNSServiceBrowse (%d)", err);
// 		return browse_arg.server_buff;
// 	}

//     int socket = DNSServiceRefSockFD(sdref);
//     set_nonblocking(socket);
//     fd_set read_fds;
//     FD_ZERO(&read_fds);
//     FD_SET(socket, &read_fds);
//     // if(select(socket+1, &read_fds, NULL, NULL, NULL)  < 0)
//     // {
//     //     SALOG_INFO("process_comm", "select");  
//     // }

//     while(1)
//     {
//         struct timeval timeout;      
//         timeout.tv_sec = 0;
//         timeout.tv_usec = 100000;
//         if(select(socket+1, &read_fds, NULL, NULL, &timeout)  <= 0) {
//             SALOG_INFO("process_comm", "select");  
//             break;
//         }
//         DNSServiceErrorType err2 = DNSServiceProcessResult(sdref);
//         // printf("err2=%d\n", err2);
//     }
//     // err = DNSServiceProcessResult(sdref);
//     // if (err != kDNSServiceErr_NoError)
//     // {
//     //     SALOG_ERROR("process_comm", "Failed DNSServiceProcessResult (%d)", err);
//     //     return server_buff;
//     // }

//     DNSServiceRefDeallocate(sdref);
//     return browse_arg.server_buff;
// }


// void PCMdns_sendtoServer(int sock, PCMdns_server_t server)
// {

// }


// static void PCMdns_serviceBrowse_callback(DNSServiceRef s, DNSServiceFlags flag, uint32_t i, DNSServiceErrorType e, const char* name, const char* regtype, const char* domain, void* args)
// {
//     if (e != kDNSServiceErr_NoError)
//     {
//         SALOG_ERROR("process_comm", "Failed PCMdns_serviceBrowse_callback (%d)", e);
// 		return;
// 	}

//     if(flag & kDNSServiceFlagsAdd)
//     {
//         PCMdns_browseArg_t* browse_arg = (PCMdns_browseArg_t*)args;
//         if(strlen(name) == browse_arg->host_len && (memcmp(name, browse_arg->host, sizeof(char) * browse_arg->host_len) == 0))
//         {
//             DNSServiceRef sr = 0;
//             DNSServiceErrorType err;

//             err = DNSServiceResolve(&sr, 0, 0, name, regtype, domain, PCMdns_serviceResolve_callback, args);
//             if (err != kDNSServiceErr_NoError)
//             {
//                 SALOG_ERROR("process_comm", "Failed DNSServiceResolve (%d)", err);
//                 return;
//             }

//             int socket = DNSServiceRefSockFD(sr);
//             set_nonblocking(socket);
//             fd_set read_fds;
//             FD_ZERO(&read_fds);
//             FD_SET(socket, &read_fds);
//             // if(select(socket+1, &read_fds, NULL, NULL, NULL)  < 0)
//             // {
//             //     SALOG_INFO("process_comm", "select");  
//             // }

//             while(1) {
//                 struct timeval timeout;      
//                 timeout.tv_sec = 0;
//                 timeout.tv_usec = 10000;
//                 int a = select(socket+1, &read_fds, NULL, NULL, &timeout);
//                 if(select(socket+1, &read_fds, NULL, NULL, &timeout)  <= 0) {
//                     // SALOG_INFO("process_comm", "select");
//                     break;
//                 }
//                 DNSServiceErrorType err2 = DNSServiceProcessResult(sr);
//                 // printf("err2=%d, %d\n", err2, a);
//             }
//             // err = DNSServiceProcessResult(sr);
//             // if (err != kDNSServiceErr_NoError)
//             // {
//             //     SALOG_ERROR("process_comm", "Failed DNSServiceProcessResult (%d)", err);
//             //     return;
//             // }

//             DNSServiceRefDeallocate(sr);
//         }
//     }
// }

// static void PCMdns_serviceResolve_callback(DNSServiceRef s, DNSServiceFlags flag, uint32_t i, DNSServiceErrorType e, const char* name, const char* host, uint16_t port, uint16_t ttl, const unsigned char* t, void* args)
// {
//     if (e != kDNSServiceErr_NoError)
//     {
//         SALOG_ERROR("process_comm", "Failed PCMdns_serviceResolve_callback (%d)", e);
// 		return;
// 	}

//     char port_str[32];
//     struct addrinfo hints;
// 	struct addrinfo* result;
// 	struct addrinfo* rp;
//     PCMdns_browseArg_t* browse_arg = (PCMdns_browseArg_t*)args;

// 	sprintf(port_str, "%d", ntohs(port));

// 	hints.ai_family = AF_INET;    /* Allow IPv4 */
// 	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
// 	hints.ai_flags = 0;
// 	hints.ai_protocol = 0;

// 	int gai_e = getaddrinfo(host, port_str, &hints, &result);
// 	if(gai_e != 0)
// 	{
// 		SALOG_ERROR("process_comm", "getaddrinfo: %s", gai_strerror(gai_e));
//         return;
// 	}

//     for (rp = result; rp != NULL; rp = rp->ai_next) 
// 	{
//         PCMdns_server_t server;
//         memcpy(&server.addr, rp->ai_addr, sizeof(struct sockaddr_in));
//         SABuffer_add(&browse_arg->server_buff, &server);

//         SALOG_INFO("process_comm", "getaddrinfo name=%s host=%s f=%d i=%d ip=%s port=%d", name, host, flag, i, inet_ntoa(server.addr.sin_addr), ntohs(port));
// 	}

// 	freeaddrinfo(result);
// }
