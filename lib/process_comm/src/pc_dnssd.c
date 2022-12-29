#ifdef __APPLE__

#include "pc_dnssd.h"

#include "pc_table.h"
#include <dns_sd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dns_sd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


static void PCMdns_serviceRegister_callback(DNSServiceRef s, DNSServiceFlags f, DNSServiceErrorType e, const char *name, const char *type, const char *domain, void *v);
static void PCMdns_serviceBrowse_callback(DNSServiceRef s, DNSServiceFlags f, uint32_t i, DNSServiceErrorType e, const char *name, const char *type, const char *domain, void *context);
static void PCMdns_serviceResolve_callback(DNSServiceRef s, DNSServiceFlags f, uint32_t i, DNSServiceErrorType e, const char *n, const char *host, uint16_t port, uint16_t tl, const unsigned char *t, void *c);
static void PCMdns_serviceGetAddrInfo_callback(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *hostname, const struct sockaddr *address, uint32_t ttl, void *context);
static void PCMdns_serviceGetAddrInfo_callback(DNSServiceRef s, DNSServiceFlags flags, uint32_t i, DNSServiceErrorType e, const char* host, const struct sockaddr* address, uint32_t ttl, void* args);


static void* PCDnssd_brwoseServer(void* arg)
{
    DNSServiceRef sdref = 0;
    DNSServiceErrorType err;

    err = DNSServiceBrowse(&sdref, 0, 0, "_rmm._udp", NULL, PCMdns_serviceBrowse_callback, NULL);
    if (err != kDNSServiceErr_NoError)
    {
        SALOG_ERROR("process_comm", "Failed DNSServiceBrowse (%d)", err);
    }

    
    while(1)
    {
        err = DNSServiceProcessResult(sdref);
        if(err != kDNSServiceErr_NoError) 
        {
            SALOG_ERROR("process_comm", "Failed DNSServiceProcessResult (%d)", err);
        }
    }

    // DNSServiceRefDeallocate(sdref);
}


void PCDnssd_init(void)
{
    pthread_t thread;
    SATHREAD_CREATE("dnns_browser", &thread, NULL, PCDnssd_brwoseServer, NULL);
}


void* PCDnssd_registServer(int sock, const struct sockaddr_in* addr_in, const char* type_name, const char* name)
{
    char service_type[256] = "_rmm._udp";

    struct sockaddr_in addr;
    if(addr_in != NULL)
    {
        memcpy(&addr, addr_in, sizeof(struct sockaddr_in));
    }
    else
    {
        socklen_t addr_len = sizeof(struct sockaddr_in);
        if(getsockname(sock, (struct sockaddr*)&addr, &addr_len) == -1)
        {
            SALOG_ERROR("process_comm", "Failed getsockname,");
        }
    }

    DNSServiceErrorType err;
    DNSServiceRef* sdref;

    sdref = (DNSServiceRef*)SAMEM_MALLOC(sizeof(DNSServiceRef));
    *sdref = 0;

    char regist_name[128];
    sprintf(regist_name, "%s(%s)", name, type_name);

    err = DNSServiceRegister(sdref, 0, 0, regist_name, service_type, NULL, NULL, addr.sin_port, 0, NULL, PCMdns_serviceRegister_callback, NULL);
	if (err != kDNSServiceErr_NoError)
    {
        SALOG_ERROR("process_comm", "Failed DNSServiceRegister (%d)", err);
        SAMEM_FREE(sdref);
		return NULL;
	}
    
    err = DNSServiceProcessResult(*sdref);
    if (err != kDNSServiceErr_NoError)
    {
        SALOG_ERROR("process_comm", "Failed DNSServiceProcessResult (%d)", err);
        SAMEM_FREE(sdref);
		return NULL;
	}

    return sdref;
}

void PCDnssd_closeServer(void* reg)
{
    DNSServiceRef* sdref = (DNSServiceRef*)reg;
    DNSServiceRefDeallocate(*sdref);
}


static void PCMdns_serviceRegister_callback(DNSServiceRef s, DNSServiceFlags f, DNSServiceErrorType e, const char *name, const char *type, const char *domain, void *v)
{
	if (e != kDNSServiceErr_NoError)
    {
        SALOG_ERROR("process_comm", "Failed PCMdns_serviceRegister_callback (%d)", e);
	}
}


static void PCMdns_serviceBrowse_callback(DNSServiceRef s, DNSServiceFlags flag, uint32_t i, DNSServiceErrorType e, const char* name, const char* regtype, const char* domain, void* args)
{
    if (e != kDNSServiceErr_NoError)
    {
        SALOG_ERROR("process_comm", "Failed PCMdns_serviceBrowse_callback (%d)", e);
		return;
	}

    SALOG_INFO("process_comm", "Pick up, %s, %d, %s, %s", name, flag, regtype, domain);

    DNSServiceRef sdref = 0;
    DNSServiceErrorType err;

    uint8_t is_add;
    if((flag & kDNSServiceFlagsAdd) != 0)
    {
        is_add = SABOOL_TRUE;
    }else{
        is_add = SABOOL_FALSE;
    }
    err = DNSServiceResolve(&sdref, 0, i, name, regtype, domain, PCMdns_serviceResolve_callback, &is_add);
    if (err != kDNSServiceErr_NoError)
    {
        SALOG_ERROR("process_comm", "Failed DNSServiceResolve (%d)", err);
        return;
    }

    err = DNSServiceProcessResult(sdref);
    if (err != kDNSServiceErr_NoError)
    {
        SALOG_ERROR("process_comm", "Failed DNSServiceProcessResult (%d)", err);
        return;
    }

    DNSServiceRefDeallocate(sdref);
}


typedef struct
{
    uint8_t is_add;
    const char* name;
    in_port_t sin_port;
} PCMdns_tmpInfo_t;

static void PCMdns_serviceResolve_callback(DNSServiceRef s, DNSServiceFlags flag, uint32_t i, DNSServiceErrorType e, const char* name, const char* host, uint16_t port, uint16_t ttl, const unsigned char* t, void* args)
{
    if (e != kDNSServiceErr_NoError)
    {
        SALOG_ERROR("process_comm", "Failed PCMdns_serviceResolve_callback (%d)", e);
		return;
	}

    DNSServiceRef sdref = 0;
    DNSServiceErrorType err;

    PCMdns_tmpInfo_t tmp_info;
    tmp_info.is_add = *(uint8_t*)args;
    tmp_info.name = name;
    tmp_info.sin_port = port;

    err = DNSServiceGetAddrInfo(&sdref, 0, i, kDNSServiceProtocol_IPv4, host, PCMdns_serviceGetAddrInfo_callback, &tmp_info);
    if (err != kDNSServiceErr_NoError)
    {
        SALOG_ERROR("process_comm", "Failed DNSServiceGetAddrInfo (%d)", err);
        return;
    }
    err = DNSServiceProcessResult(sdref);
    if (err != kDNSServiceErr_NoError)
    {
        SALOG_ERROR("process_comm", "Failed DNSServiceProcessResult (%d)", err);
        return;
    }
    DNSServiceRefDeallocate(sdref);
}

static void PCMdns_serviceGetAddrInfo_callback(DNSServiceRef s, DNSServiceFlags flags, uint32_t i, DNSServiceErrorType e, const char* host, const struct sockaddr* address, uint32_t ttl, void* args)
{
    if (e != kDNSServiceErr_NoError)
    {
        SALOG_ERROR("process_comm", "Failed PCMdns_serviceResolve_callback (%d)", e);
        return;
    }

    PCMdns_tmpInfo_t* tmp_info = (PCMdns_tmpInfo_t*)args;

    struct sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_addr = ((struct sockaddr_in*)address)->sin_addr;
    addr_in.sin_port = tmp_info->sin_port;

    char server_name[256];
    char server_type[256];
    size_t name_len = strlen(tmp_info->name);
    if(name_len < 256)
    {
        size_t i = 0;
        for(; i < name_len && tmp_info->name[i] != '('; i++) {}
        memcpy(server_name, tmp_info->name, sizeof(char) * i);
        server_name[i] = '\0';
        i++;

        size_t offset = i;
        for(; i < name_len && tmp_info->name[i] != ')'; i++) {}
        memcpy(server_type, &tmp_info->name[offset], sizeof(char) * (i - offset));
        server_type[i - offset] = '\0';

        if(tmp_info->is_add == SABOOL_TRUE)
        {
            PCTable_setSockaddr(server_type, server_name, &addr_in);
        }else{
            PCTable_deactivate(server_type, server_name, &addr_in);
        }
    }
}

#endif /*__APPLE__*/
