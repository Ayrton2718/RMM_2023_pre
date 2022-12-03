#include "cs_io.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "cs_endian.h"

// #define CSIO_LOCAL_DEBUG (1)

#ifdef CSIO_LOCAL_DEBUG
    #define CSIO_CONVERTER_IP_ADDR	"127.0.0.1"
#else /*CSIO_LOCAL_DEBUG*/
    #define CSIO_CONVERTER_IP_ADDR	"172.22.4.11"
#endif /*CSIO_LOCAL_DEBUG*/

in_addr_t CSIo_createIp(size_t gw_i)
{
    static const char* str_ips[4] = {
        "172.22.4.11",
        "172.22.4.12",
        "172.22.4.13",
        "172.22.4.14",
    };
    return inet_addr(str_ips[gw_i]);
}


#define CSIO_PING_STRING	"Hello"

#define CSIO_DATA_RECV_PORT 	(20101)
#define CSIO_DATA_SEND_PORT 	(20100)
#define CSIO_ERROR_SEND_PORT    (20111)
#define CSIO_PING_RECV_PORT 	(20121)
#define CSIO_PING_SEND_PORT 	(20120)

#define CSIO_SEQUENCE_MAP_COUNT  (4)

struct CSIo_info_t
{
    int sock;

    struct sockaddr_in  send_addrs[4];
    in_addr_t           recv_ips[4];

    uint8_t send_sequence[4];

    uint8_t recv_next_map_index[4];
    uint8_t recv_sequence_map[4][CSIO_SEQUENCE_MAP_COUNT];
};

struct CSIo_dataFrame_t
{
    uint16_t id;
    uint8_t reg;
    uint8_t len;
    uint8_t data[4];
} __attribute__((__packed__));

struct CSIo_ethFrame_t
{
    uint8_t sequence;
    uint8_t data_count;
    struct CSIo_dataFrame_t data_frame[CSIO_DATAPACK_COUNT];
} __attribute__((__packed__));


static struct CSIo_info_t g_obj;


void CSIo_initializer(void)
{
    g_obj.sock = SASocket_socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in recv_addr;

    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = htons(CSIO_DATA_RECV_PORT);
    recv_addr.sin_addr.s_addr = INADDR_ANY;

    SASocket_bind(g_obj.sock, (struct sockaddr*)&recv_addr, sizeof(recv_addr));

    for(size_t gw_i = 0; gw_i < 4; gw_i++)
    {
        g_obj.send_addrs[gw_i].sin_family = AF_INET;
        g_obj.send_addrs[gw_i].sin_port = htons(CSIO_DATA_SEND_PORT);
        g_obj.send_addrs[gw_i].sin_addr.s_addr = CSIo_createIp(gw_i);

        g_obj.recv_ips[gw_i] = CSIo_createIp(gw_i);

        g_obj.send_sequence[gw_i] = 0;

        g_obj.recv_next_map_index[gw_i] = 0;
        memset(g_obj.recv_sequence_map[gw_i], UINT8_MAX, sizeof(uint8_t) * CSIO_SEQUENCE_MAP_COUNT);
    }
}


SABool_t CSIo_connectionWait(size_t gw_id)
{
    int sock = SASocket_socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in recv_addr;
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = htons(CSIO_PING_RECV_PORT);
    recv_addr.sin_addr.s_addr = INADDR_ANY;

    while(SASocket_bind(sock, (struct sockaddr*)&recv_addr, sizeof(recv_addr)) < 0)
    {
    }

    SALOG_INFO("can_smbus", "Gateway boot waiting...");

    struct sockaddr_in client_address;
	socklen_t client_address_len = sizeof(client_address);

    while(1)
    {
        char buff[128];
        if(SASocket_recvfrom(sock, buff, sizeof(buff), 0, (struct sockaddr *)&client_address, &client_address_len) == sizeof(CSIO_PING_STRING))
        {
            // SALOG_INFO("can_smbus", "ip: %s", inet_ntoa(client_address.sin_addr));

            if(CSIo_createIp(gw_id) == client_address.sin_addr.s_addr)
            {
                buff[100] = '\0';
                if(strcmp(buff, CSIO_PING_STRING) == 0)
                {
                    break;
                }
            }
        }
    }

    SASocket_close(sock);

    return SABOOL_TRUE;
}


SABool_t CSIo_read(struct CSIo_dataPack_t* data_pack)
{
    struct sockaddr_in client_address;
	socklen_t client_address_len = sizeof(client_address);

    data_pack->data_count = 0;

    char buff[128];
    if(SASocket_recvfrom(g_obj.sock, buff, sizeof(buff), 0, (struct sockaddr *)&client_address, &client_address_len) == sizeof(struct CSIo_ethFrame_t))
    {
        for(size_t gw_i = 0; gw_i < 4; gw_i++)
        {
            if(g_obj.recv_ips[gw_i] == client_address.sin_addr.s_addr)
            {
                struct CSIo_ethFrame_t* eth_frame = (struct CSIo_ethFrame_t*)buff;

                uint8_t is_effective = 1;
                for(size_t i = 0; i < CSIO_SEQUENCE_MAP_COUNT; i++)
                {
                    if(g_obj.recv_sequence_map[gw_i][i] == eth_frame->sequence)
                    {
                        is_effective = 0;
                        break;
                    }
                }

                if(is_effective)
                {
                    data_pack->gw_id = gw_i;
                    data_pack->data_count = eth_frame->data_count;
                    for(size_t i = 0; i < data_pack->data_count; i++)
                    {
                        data_pack->data[i].id = le16toh(eth_frame->data_frame[i].id);
                        data_pack->data[i].reg = eth_frame->data_frame[i].reg;
                        data_pack->data[i].len = eth_frame->data_frame[i].len;
                        memcpy(data_pack->data[i].data, eth_frame->data_frame[i].data, sizeof(uint8_t) * 4);
                    }

                    g_obj.recv_sequence_map[gw_i][g_obj.recv_next_map_index[gw_i] % CSIO_SEQUENCE_MAP_COUNT] = eth_frame->sequence;
                    g_obj.recv_next_map_index[gw_i]++;

                    return SABOOL_TRUE;
                }

                break;
            }
        }
    }

    return SABOOL_FALSE;
}


void CSIo_send(const struct CSIo_dataPack_t* data_pack)
{
    struct CSIo_ethFrame_t eth_frame;

    eth_frame.sequence = g_obj.send_sequence[data_pack->gw_id];
    g_obj.send_sequence[data_pack->gw_id]++;

    eth_frame.data_count = data_pack->data_count;

    for(size_t i = 0; i < eth_frame.data_count; i++)
    {
        eth_frame.data_frame[i].id = htole16(data_pack->data[i].id);
        eth_frame.data_frame[i].reg = data_pack->data[i].reg;
        eth_frame.data_frame[i].len = data_pack->data[i].len;
        memcpy(eth_frame.data_frame[i].data, data_pack->data[i].data, sizeof(uint8_t) * 4);
    }

// #ifdef CS_DEBUG_ON
//     SALOG_INFO("can_smbus", "Send Pack [%x, %x, %x, %x]", eth_frame.data_frame[0].id, eth_frame.data_frame[1].id, eth_frame.data_frame[2].id, eth_frame.data_frame[3].id);
    // SALOG_INFO("can_smbus", "Send to 0x%03x, 0x%02x, %d [0x%02x, 0x%02x, 0x%02x, 0x%02x]", eth_frame.data_frame[0].id, eth_frame.data_frame[0].reg, eth_frame.data_frame[0].len, eth_frame.data_frame[0].data[0], eth_frame.data_frame[0].data[1], eth_frame.data_frame[0].data[2], eth_frame.data_frame[0].data[3]);
// #endif /*CS_DEBUG_ON*/
    SASocket_sendto(g_obj.sock, &eth_frame, sizeof(struct CSIo_ethFrame_t), 0, (struct sockaddr*)&g_obj.send_addrs[data_pack->gw_id], sizeof(struct sockaddr_in));
    // sendto(g_obj.sock, &eth_frame, sizeof(struct CSIo_ethFrame_t), 0, (struct sockaddr*)&g_obj.send_addr, sizeof(g_obj.send_addr));
}