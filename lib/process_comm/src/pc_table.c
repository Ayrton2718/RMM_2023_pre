#include "pc_table.h"

#include "pc_type.h"
#include <arpa/inet.h>

typedef struct
{
    SABool_t            is_active;
    struct sockaddr_in  addr;
} PCTable_addr_t;

typedef struct
{
    uint32_t            hash;
    char                service[64];
    char                name[128];
    SABuffer_t          addrs;
} PCTable_service_t;


typedef struct
{
    pthread_mutex_t locker;
    SABuffer_t services;
} PCTable_t;


static PCTable_t g_obj;

static PCTable_service_t* PCTable_search_nolock(const char* service_type, const char* name);


void PCTable_init(void)
{
    SALOCKER_INIT(&g_obj.locker, NULL);
    SABuffer_create(&g_obj.services, sizeof(PCTable_service_t));
}


void PCTable_set(const char* service_type, const char* name, const char* ip, uint16_t port)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, (void*)&addr.sin_addr);
    PCTable_setSockaddr(service_type, name, &addr);
}


void PCTable_setSockaddr(const char* service_type, const char* name, const struct sockaddr_in* addr)
{
    size_t service_len = strlen(service_type);
    if(64 <= service_len)
    {
        SALOG_ERROR("process_comm", "Service type is over length! \"%s\"", service_type);
        service_len = 63;
    }

    size_t name_len = strlen(name);
    if(128 <= name_len)
    {
        SALOG_ERROR("process_comm", "name's length overed! \"%s\"", name);
        service_len = 127;
    }

    SALOCKER_LOCK(&g_obj.locker);
    PCTable_service_t* search = PCTable_search_nolock(service_type, name);
    if(search != NULL)
    {
        size_t addrs_len = SABuffer_count(&search->addrs);
        PCTable_addr_t* addr_raw = (PCTable_addr_t*)SABuffer_getRaw(&search->addrs);

        SABool_t is_hit = SABOOL_FALSE;
        for(size_t i = 0; i < addrs_len; i++)
        {
            if(addr_raw[i].addr.sin_family == AF_INET
                && addr_raw[i].addr.sin_addr.s_addr == addr->sin_addr.s_addr
                && addr_raw[i].addr.sin_port == addr->sin_port)
            {
                is_hit = SABOOL_TRUE;
                addr_raw[i].is_active = SABOOL_TRUE;
                break;
            }
        }

        if(is_hit == SABOOL_FALSE)
        {
            PCTable_addr_t addr_table;
            addr_table.is_active = SABOOL_TRUE;
            memcpy(&addr_table.addr, addr, sizeof(struct sockaddr_in));
            SABuffer_add(&search->addrs, &addr_table);
        }
    }
    else
    {
        PCTable_service_t service;
        service.hash = service_type[0] + name[0];
        memcpy(service.name, name, sizeof(char) * name_len);
        service.name[name_len] = '\0';
        memcpy(service.service, service_type, sizeof(char) * service_len);
        service.service[service_len] = '\0';
        SABuffer_create(&service.addrs, sizeof(PCTable_addr_t));

        PCTable_addr_t addr_table;
        addr_table.is_active = SABOOL_TRUE;
        memcpy(&addr_table.addr, addr, sizeof(struct sockaddr_in));
        SABuffer_add(&service.addrs, &addr_table);

        SABuffer_add(&g_obj.services, &service);
    }
    SALOCKER_UNLOCK(&g_obj.locker);
}


void PCTable_deactivate(const char* service_type, const char* name, const struct sockaddr_in* addr)
{
    size_t service_len = strlen(service_type);
    if(64 <= service_len)
    {
        SALOG_ERROR("process_comm", "Service type is over length! \"%s\"", service_type);
        service_len = 63;
    }

    size_t name_len = strlen(name);
    if(128 <= name_len)
    {
        SALOG_ERROR("process_comm", "name's length overed! \"%s\"", name);
        service_len = 127;
    }

    SALOCKER_LOCK(&g_obj.locker);
    PCTable_service_t* search = PCTable_search_nolock(service_type, name);
    if(search != NULL)
    {
        size_t addrs_len = SABuffer_count(&search->addrs);
        PCTable_addr_t* addr_raw = (PCTable_addr_t*)SABuffer_getRaw(&search->addrs);

        SABool_t is_hit = SABOOL_FALSE;
        for(size_t i = 0; i < addrs_len; i++)
        {
            if(addr_raw[i].addr.sin_family == AF_INET
                && addr_raw[i].addr.sin_addr.s_addr == addr->sin_addr.s_addr
                && addr_raw[i].addr.sin_port == addr->sin_port)
            {
                is_hit = SABOOL_TRUE;
                addr_raw[i].is_active = SABOOL_FALSE;
                break;
            }
        }

        if(is_hit == SABOOL_FALSE)
        {
            SALOG_ERROR("process_comm", "Can't find %s(%s)!", search->name, search->service);
        }
    }else{
        SALOG_ERROR("process_comm", "%s(%s) can't find!", name, service_type);
    }
    SALOCKER_UNLOCK(&g_obj.locker);
}


PCTable_instance_t PCTable_loockUp(const char* service_type, const char* name)
{
    size_t service_len = strlen(service_type);
    if(64 <= service_len)
    {
        SALOG_ERROR("process_comm", "Service type is over length! \"%s\"", service_type);
        service_len = 63;
    }

    size_t name_len = strlen(name);
    if(128 <= name_len)
    {
        SALOG_ERROR("process_comm", "name's length overed! \"%s\"", name);
        service_len = 127;
    }

    SALOCKER_LOCK(&g_obj.locker);
    PCTable_service_t* search = PCTable_search_nolock(service_type, name);
    SALOCKER_UNLOCK(&g_obj.locker);

    if(search == NULL)
    {
        SALOG_ERROR("process_comm", "%s(%s) can't find!", name, service_type);
    }

    return search;
}

struct sockaddr_in PCTable_getIp(PCTable_instance_t instance)
{
    struct sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = 0;
    inet_pton(AF_INET, "0.0.0.0", (void*)&addr_in.sin_addr);

    SALOCKER_LOCK(&g_obj.locker);
    PCTable_service_t* _instance = (PCTable_service_t*)instance;
    if(_instance != NULL)
    {
        size_t addrs_len = SABuffer_count(&_instance->addrs);
        PCTable_addr_t* addr_raw = (PCTable_addr_t*)SABuffer_getRaw(&_instance->addrs);

        SABool_t is_hit = SABOOL_FALSE;
        for(size_t i = 0; i < addrs_len; i++)
        {
            if(addr_raw[i].is_active == SABOOL_TRUE)
            {
                memcpy(&addr_in, &addr_raw[i].addr, sizeof(struct sockaddr_in));
                is_hit = SABOOL_TRUE;
                break;
            }
        }

        if(is_hit == SABOOL_FALSE)
        {
            SALOG_ERROR("process_comm", "Can't find %s(%s)!", _instance->name, _instance->service);
        }
    }else{
        SALOG_ERROR("process_comm", "Instance is null");
    }
    SALOCKER_UNLOCK(&g_obj.locker);

    return addr_in;
}


static PCTable_service_t* PCTable_search_nolock(const char* service_type, const char* name)
{
    uint32_t hash = service_type[0] + name[0];

    size_t len = SABuffer_count(&g_obj.services);
    PCTable_service_t* services = (PCTable_service_t*)SABuffer_getRaw(&g_obj.services);
    for(size_t service_i = 0; service_i < len; service_i++)
    {
        if(services[service_i].hash == hash)
        {
            size_t type_len = strlen(service_type);
            if(memcmp(services[service_i].service, service_type, sizeof(char) * type_len) == 0)
            {
                size_t name_len = strlen(name);
                if(memcmp(services[service_i].name, name, sizeof(char) * name_len) == 0)
                {
                    return &services[service_i];
                }
            }
        }
    }
    return NULL;
}

