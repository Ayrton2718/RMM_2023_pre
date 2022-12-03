#include "rm_m3508.h"

#include <process_comm/process_comm.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#ifdef ROBOMAS_MOTOR_LOCAL_DEBUG
    #define ROBOMAS_MOTOR_CONVERTER_IP_ADDR	"127.0.0.1"
#else /*ROBOMAS_MOTOR_LOCAL_DEBUG*/
    #define ROBOMAS_MOTOR_CONVERTER_IP_ADDR	"172.22.4.11"
#endif /*ROBOMAS_MOTOR_LOCAL_DEBUG*/

#define ROBOMAS_MOTOR_RECV_PORT 	    (20522)     // 受信ポート
#define ROBOMAS_MOTOR_POWER_SEND_PORT 	(20520)     // 出力送信ポート
#define ROBOMAS_MOTOR_CONF_SEND_PORT 	(20521)     // 設定送信ポート

struct RM_ethCoefFrame_t
{
	uint8_t mot_id;
	uint32_t p;
	uint32_t i;
	uint32_t d;
} __attribute__((__packed__));

struct RM_ethAdvancedCoefFrame_t
{
	uint8_t mot_id;
	uint32_t p;
	uint32_t i;
	uint32_t d;
    int8_t  inverse;
} __attribute__((__packed__));

struct RMM3508_ethFrame_t
{
	uint8_t mot_id;
	int16_t value;
    // uint8_t is_rpm;
} __attribute__((__packed__));


// 新しい出力送信フレーム互換性を維持するために，前のも残してる．
struct RMM3508_ethRpmSyncFrame_t
{
	uint8_t mot_id1;
	uint8_t mot_id2;
	int16_t rpm;
} __attribute__((__packed__));

// 新しい出力送信フレーム互換性を維持するために，前のも残してる．
struct RMM3508_ethNewFrame_t
{
	uint8_t mot_id;
    uint8_t power_mode; // 0 : rpm, 1 : current.
	int16_t value;
	uint8_t mot_type; // 0 : m3508, 1 : m2006
} __attribute__((__packed__));

struct RM_ethPitchFrame_t
{
	uint8_t mot_id;
	int16_t base_power;
	int64_t target_rota;
} __attribute__((__packed__));

struct RM_ethSensData_t
{
	int16_t rpm;
	int16_t cur;
	int16_t set_cur;
	int64_t rota; // [2PI / 10]
    uint8_t status;
} __attribute__((__packed__));


struct Robomas_motor_info_t
{
    size_t gw_count;

    pthread_mutex_t locker;

    int sock;
    struct sockaddr_in rpm_send_addr[4];
    struct sockaddr_in conf_send_addr[4];

    CC_obj sensors[4];

    CC_obj is_safety;
};

static struct Robomas_motor_info_t g_obj;


void RMM3508_sensThread(void);
void RMM3508_safetyThread(void);

const char* RMM3508_createIp(size_t gw_i)
{
    static char ip_str[128];
    sprintf(ip_str, "172.22.4.%d", (int)(11 + gw_i));
    return ip_str;
}


void RMM3508_init(size_t gw_count)
{
    g_obj.gw_count = gw_count;

    SALOCKER_INIT(&g_obj.locker, NULL);

    g_obj.sock = SASocket_socket(AF_INET, SOCK_DGRAM, 0);

    for(size_t gw_i = 0; gw_i < g_obj.gw_count; gw_i++)
    {
        g_obj.rpm_send_addr[gw_i].sin_family = AF_INET;
        g_obj.rpm_send_addr[gw_i].sin_port = htons(ROBOMAS_MOTOR_POWER_SEND_PORT);
        g_obj.rpm_send_addr[gw_i].sin_addr.s_addr = inet_addr(RMM3508_createIp(gw_i));

        g_obj.conf_send_addr[gw_i].sin_family = AF_INET;
        g_obj.conf_send_addr[gw_i].sin_port = htons(ROBOMAS_MOTOR_CONF_SEND_PORT);
        g_obj.conf_send_addr[gw_i].sin_addr.s_addr = inet_addr(RMM3508_createIp(gw_i));

        g_obj.sensors[gw_i] = CCStruct_create(NULL, sizeof(struct RM_ethSensData_t) * 8);
        CCObject_enableThreadSafe(g_obj.sensors[gw_i]);
    }

    g_obj.is_safety = CCBool_create(SABOOL_TRUE);
    CCObject_enableThreadSafe(g_obj.is_safety);

    TCLauncher_launch(RMM3508_sensThread, "robomas_motor_sens");
    TCLauncher_launch(RMM3508_safetyThread, "robomas_motor_safety");
}


void RMM3508_setCoef(size_t gw_id, int number, float p, float i, float d)
{
    struct RM_ethCoefFrame_t pid_conf;

    pid_conf.mot_id = number - 1;
    pid_conf.p = (uint32_t)(p * 10000);
    pid_conf.i = (uint32_t)(i * 10000);
    pid_conf.d = (uint32_t)(d * 10000);

    int sock = PCMdns_createSocket(PCMdns_protocol_Udp, NULL);
#ifdef __APPLE__
    struct timeval tv = {.tv_sec = 1, .tv_usec = 0};
    // tv.tv_sec = 1
#else
    struct timeval tv = {.tv_sec = 0, .tv_usec = 50000};
#endif /*__APPLE__*/
    SASocket_setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(struct timeval));

    struct sockaddr_in client_address;
	socklen_t client_address_len = sizeof(struct sockaddr_in);

    size_t error_count = 0;
    while(1)
    {
        SASocket_sendto(sock, &pid_conf, sizeof(struct RM_ethCoefFrame_t), 0, (struct sockaddr*)&g_obj.conf_send_addr[gw_id], sizeof(struct sockaddr_in));

        struct RM_ethCoefFrame_t ack_conf;
        size_t recv_len = SASocket_recvfrom(sock, &ack_conf, sizeof(struct RM_ethCoefFrame_t), 0, (struct sockaddr*)&client_address, &client_address_len);
        if(recv_len == sizeof(struct RM_ethCoefFrame_t))
        {
            uint8_t diff_sum = 0;
            diff_sum += ((ack_conf.mot_id == pid_conf.mot_id)? 0:1);
            diff_sum += ((ack_conf.p == pid_conf.p)? 0:1);
            diff_sum += ((ack_conf.i == pid_conf.i)? 0:1);
            diff_sum += ((ack_conf.d == pid_conf.d)? 0:1);
            if(diff_sum == 0)
            {
                break;
            }
        }

        if(1 < error_count)
        {
            SALOG_ERROR("roboms", "Failed to RMM3508_setCoef!");
            break;
        }
        SALOG_WARN("roboms", "Timeout! Retrying...");
        error_count++;
    }

    close(sock);
}

void RMM3508_setAdvancedCoef(size_t gw_id, int number, float p, float i, float d, SABool_t inverse)
{
    struct RM_ethAdvancedCoefFrame_t adv_conf;

    adv_conf.mot_id = number - 1;
    adv_conf.p = (uint32_t)(p * 10000);
    adv_conf.i = (uint32_t)(i * 10000);
    adv_conf.d = (uint32_t)(d * 10000);
    if(inverse == SABOOL_FALSE)
    {
        adv_conf.inverse = 1;
    }else{
        adv_conf.inverse = -1;
    }

    int sock = PCMdns_createSocket(PCMdns_protocol_Udp, NULL);
#ifdef __APPLE__
    struct timeval tv = {.tv_sec = 1, .tv_usec = 0};
    // tv.tv_sec = 1
#else
    struct timeval tv = {.tv_sec = 0, .tv_usec = 50000};
#endif /*__APPLE__*/
    SASocket_setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(struct timeval));

    struct sockaddr_in client_address;
	socklen_t client_address_len = sizeof(struct sockaddr_in);

    size_t error_count = 0;
    while(1)
    {
        SASocket_sendto(sock, &adv_conf, sizeof(struct RM_ethAdvancedCoefFrame_t), 0, (struct sockaddr*)&g_obj.conf_send_addr[gw_id], sizeof(struct sockaddr_in));

        struct RM_ethAdvancedCoefFrame_t ack_conf;
        size_t recv_len = SASocket_recvfrom(sock, &ack_conf, sizeof(struct RM_ethAdvancedCoefFrame_t), 0, (struct sockaddr*)&client_address, &client_address_len);
        if(recv_len == sizeof(struct RM_ethAdvancedCoefFrame_t))
        {
            uint8_t diff_sum = 0;
            diff_sum += ((ack_conf.mot_id == adv_conf.mot_id)? 0:1);
            diff_sum += ((ack_conf.p == adv_conf.p)? 0:1);
            diff_sum += ((ack_conf.i == adv_conf.i)? 0:1);
            diff_sum += ((ack_conf.d == adv_conf.d)? 0:1);
            diff_sum += ((ack_conf.inverse == adv_conf.inverse)? 0:1);
            if(diff_sum == 0)
            {
                break;
            }
        }

        if(3 < error_count)
        {
            SALOG_ERROR("roboms", "Failed to RMM3508_setCoef!");
            break;
        }
        SALOG_WARN("roboms", "Timeout! Retrying...");
        error_count++;
    }

    close(sock);
}


void RMM3508_safetyOn(void)
{
    CCBool_set(g_obj.is_safety, SABOOL_TRUE);
}

void RMM3508_safetyOff(void)
{
    CCBool_set(g_obj.is_safety, SABOOL_FALSE);
}


void RMM3508_setRpmAsM3508(size_t gw_id, int number, int16_t rpm)
{
    if(gw_id < g_obj.gw_count && 0 < number && number <= 8)
    {
        struct RMM3508_ethNewFrame_t frame;

        if(10000 < rpm)
        {
            rpm = 10000;
        }else if(rpm < -10000){
            rpm = -10000;
        }
    
        frame.mot_id = number - 1;
        frame.mot_type = 0;
        frame.power_mode = 0;
        frame.value = rpm;

        SALOCKER_LOCK(&g_obj.locker);
        SASocket_sendto(g_obj.sock, &frame, sizeof(struct RMM3508_ethNewFrame_t), 0, (struct sockaddr*)&g_obj.rpm_send_addr[gw_id], sizeof(struct sockaddr_in));
        SALOCKER_UNLOCK(&g_obj.locker);
    }else{
        SALOG_ERROR("robomas", "Invalid gw_id(%d) or number(%d).", gw_id, number);
    }
}

void RMM3508_setCurrentAsM3508(size_t gw_id, int number, float current)
{
    if(gw_id < g_obj.gw_count && 0 < number && number <= 8)
    {
        struct RMM3508_ethNewFrame_t frame;

        if(20 < current)
        {
            current = 20;
        }else if(current < -20)
        {
            current = -20;
        }
        
        frame.mot_id = number - 1;
        frame.mot_type = 0;
        frame.power_mode = 1;
        frame.value = current * ((float)16384 / (float)20);

        SALOCKER_LOCK(&g_obj.locker);
        SASocket_sendto(g_obj.sock, &frame, sizeof(struct RMM3508_ethNewFrame_t), 0, (struct sockaddr*)&g_obj.rpm_send_addr[gw_id], sizeof(struct sockaddr_in));
        SALOCKER_UNLOCK(&g_obj.locker);
    }else{
        SALOG_ERROR("robomas", "Invalid gw_id(%d) or number(%d).", gw_id, number);
    }
}

void RMM3508_setRpmSyncAsM3508(size_t gw_id, int number1, int number2, int16_t rpm)
{
    if(gw_id < g_obj.gw_count && 0 < number1 && number1 <= 8 && 0 < number2 && number2 <= 8)
    {
        struct RMM3508_ethRpmSyncFrame_t frame;

        if(10000 < rpm)
        {
            rpm = 10000;
        }else if(rpm < -10000){
            rpm = -10000;
        }
    
        frame.mot_id1 = number1 - 1;
        frame.mot_id2 = number2 - 1;
        frame.rpm = rpm;

        SALOCKER_LOCK(&g_obj.locker);
        SASocket_sendto(g_obj.sock, &frame, sizeof(struct RMM3508_ethRpmSyncFrame_t), 0, (struct sockaddr*)&g_obj.rpm_send_addr[gw_id], sizeof(struct sockaddr_in));
        SALOCKER_UNLOCK(&g_obj.locker);
    }else{
        SALOG_ERROR("robomas", "Invalid gw_id(%d) or number1(%d), number2(%d).", gw_id, number1, number2);
    }
}

void RMM3508_setPitchAsM3508(size_t gw_id, int number, float base_cur, float target_rote)
{
    if(gw_id < g_obj.gw_count && 0 < number && number <= 8)
    {
        struct RM_ethPitchFrame_t frame;

        frame.mot_id = number - 1;

        if(20 < base_cur)
        {
            base_cur = 20;
        }else if(base_cur < -20)
        {
            base_cur = -20;
        }
        
        frame.base_power = base_cur * ((float)16384 / (float)20);

        double roate = (((double)3591 / (double)187) * (double)target_rote);
        frame.target_rota = (int64_t)(roate * 1000);

        SALOCKER_LOCK(&g_obj.locker);
        SASocket_sendto(g_obj.sock, &frame, sizeof(struct RM_ethPitchFrame_t), 0, (struct sockaddr*)&g_obj.rpm_send_addr[gw_id], sizeof(struct sockaddr_in));
        SALOCKER_UNLOCK(&g_obj.locker);
    }else{
        SALOG_ERROR("robomas", "Invalid gw_id(%d) or number1(%d).", gw_id, number);
    }
}


void RMM3508_setRpmAsM2006(size_t gw_id, int number, int16_t rpm)
{
    if(gw_id < g_obj.gw_count && 0 < number && number <= 8)
    {
        struct RMM3508_ethNewFrame_t frame;

        if(22000 < rpm)
        {
            rpm = 22000;
        }else if(rpm < -22000){
            rpm = -22000;
        }
    
        frame.mot_id = number - 1;
        frame.mot_type = 1;
        frame.power_mode = 0;
        frame.value = rpm;

        SALOCKER_LOCK(&g_obj.locker);
        SASocket_sendto(g_obj.sock, &frame, sizeof(struct RMM3508_ethNewFrame_t), 0, (struct sockaddr*)&g_obj.rpm_send_addr[gw_id], sizeof(struct sockaddr_in));
        SALOG_ERROR("robomas", "Invalid ip : %s gw_id(%d) or number(%d).", inet_ntoa(g_obj.rpm_send_addr[gw_id].sin_addr), gw_id, number);
        SALOCKER_UNLOCK(&g_obj.locker);
    }else{
        SALOG_ERROR("robomas", "Invalid gw_id(%d) or number(%d).", gw_id, number);
    }
}

void RMM3508_setCurrentAsM2006(size_t gw_id, int number, float current)
{
    if(gw_id < g_obj.gw_count && 0 < number && number <= 8)
    {
        struct RMM3508_ethNewFrame_t frame;

        if(10 < current)
        {
            current = 10;
        }else if(current < -10)
        {
            current = -10;
        }
        
        frame.mot_id = number - 1;
        frame.mot_type = 1;
        frame.power_mode = 1;
        frame.value = current * ((float)10000 / (float)10);

        SALOCKER_LOCK(&g_obj.locker);
        SASocket_sendto(g_obj.sock, &frame, sizeof(struct RMM3508_ethNewFrame_t), 0, (struct sockaddr*)&g_obj.rpm_send_addr[gw_id], sizeof(struct sockaddr_in));
        SALOCKER_UNLOCK(&g_obj.locker);
    }else{
        SALOG_ERROR("robomas", "Invalid gw_id(%d) or number(%d).", gw_id, number);
    }
}


int16_t RMM3508_getRpm(size_t gw_id, int number)
{
    if(gw_id < g_obj.gw_count && 0 < number && number <= 8)
    {
        struct RM_ethSensData_t data[8];
        CCStruct_get(g_obj.sensors[gw_id], data, sizeof(struct RM_ethSensData_t) * 8);
        return data[number - 1].rpm;
    }else{
        SALOG_ERROR("robomas", "Invalid gw_id(%d) or number(%d).", gw_id, number);
        return 0;
    }
}


float RMM3508_getRealCurrentAsM3508(size_t gw_id, int number)
{
    if(gw_id < g_obj.gw_count && 0 < number && number <= 8)
    {
        struct RM_ethSensData_t data[8];
        CCStruct_get(g_obj.sensors[gw_id], data, sizeof(struct RM_ethSensData_t) * 8);
        return (float)data[number - 1].cur * ((float)20 / (float)16384);
    }else{
        SALOG_ERROR("robomas", "Invalid gw_id(%d) or number(%d).", gw_id, number);
        return 0;
    }
}

float RMM3508_getOrderCurrentAsM3508(size_t gw_id, int number)
{
    if(gw_id < g_obj.gw_count && 0 < number && number <= 8)
    {
        struct RM_ethSensData_t data[8];
        CCStruct_get(g_obj.sensors[gw_id], data, sizeof(struct RM_ethSensData_t) * 8);
        return (float)data[number - 1].set_cur * ((float)20 / (float)16384);
    }else{
        SALOG_ERROR("robomas", "Invalid gw_id(%d) or number(%d).", gw_id, number);
        return 0;
    }
}


float RMM3508_getRealCurrentAsM2006(size_t gw_id, int number)
{
    if(gw_id < g_obj.gw_count && 0 < number && number <= 8)
    {
        struct RM_ethSensData_t data[8];
        CCStruct_get(g_obj.sensors[gw_id], data, sizeof(struct RM_ethSensData_t) * 8);
        return (float)data[number - 1].cur * ((float)10 / (float)10000);
    }else{
        SALOG_ERROR("robomas", "Invalid gw_id(%d) or number(%d).", gw_id, number);
        return 0;
    }
}

float RMM3508_getOrderCurrentAsM2006(size_t gw_id, int number)
{
    if(gw_id < g_obj.gw_count && 0 < number && number <= 8)
    {
        struct RM_ethSensData_t data[8];
        CCStruct_get(g_obj.sensors[gw_id], data, sizeof(struct RM_ethSensData_t) * 8);
        return (float)data[number - 1].set_cur * ((float)10 / (float)10000);
    }else{
        SALOG_ERROR("robomas", "Invalid gw_id(%d) or number(%d).", gw_id, number);
        return 0;
    }
}

float RMM3508_getTheta(size_t gw_id, int number)
{
    if(gw_id < g_obj.gw_count && 0 < number && number <= 8)
    {
        struct RM_ethSensData_t data[8];
        CCStruct_get(g_obj.sensors[gw_id], data, sizeof(struct RM_ethSensData_t) * 8);

        double rota = (float)data[number - 1].rota;

        return (rota / 1000) * ((double)187 / (double)3591);
    }else{
        SALOG_ERROR("robomas", "Invalid gw_id(%d) or number(%d).", gw_id, number);
        return 0;
    }
}


SABool_t RMM3508_isWakeup(size_t gw_id, int number)
{
    if(gw_id < g_obj.gw_count && 0 < number && number <= 8)
    {
        struct RM_ethSensData_t data[8];
        CCStruct_get(g_obj.sensors[gw_id], data, sizeof(struct RM_ethSensData_t) * 8);
        if(data[number - 1].status != 0)
        {
            return SABOOL_TRUE;
        }else{
            return SABOOL_FALSE;
        }
    }else{
        SALOG_ERROR("robomas", "Invalid gw_id(%d) or number(%d).", gw_id, number);
        return SABOOL_FALSE;
    }
}


void RMM3508_sensThread(void)
{
    int sock = SASocket_socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in recv_addr;

    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = htons(ROBOMAS_MOTOR_RECV_PORT);
    recv_addr.sin_addr.s_addr = INADDR_ANY;

    SASocket_bind(sock, (struct sockaddr*)&recv_addr, sizeof(recv_addr));

	struct sockaddr_in client_address;
	socklen_t client_address_len = sizeof(client_address);

    size_t gw_count = 0;

    SALOCKER_LOCK(&g_obj.locker);
    gw_count = g_obj.gw_count;
    
    in_addr_t gw_ips[4];
    for(size_t gw_i = 0; gw_i < gw_count; gw_i++)
    {
        gw_ips[gw_i] = inet_addr(RMM3508_createIp(gw_i));
    }
    SALOCKER_UNLOCK(&g_obj.locker);

    while(1)
    {
        uint8_t buff[128];
        if(SASocket_recvfrom(sock, buff, sizeof(buff), 0, (struct sockaddr *)&client_address, &client_address_len) == (sizeof(struct RM_ethSensData_t) * 8))
        {
            for(size_t gw_i = 0; gw_i < gw_count; gw_i++)
            {
                if(gw_ips[gw_i] == client_address.sin_addr.s_addr)
                {
                    struct RM_ethSensData_t* frame = (struct RM_ethSensData_t*)buff;
                    CCStruct_set(g_obj.sensors[gw_i], frame, sizeof(struct RM_ethSensData_t) * 8);
                    // printf("%d,\t%d,\t%d,\t%d\n", frame[0].rpm, frame[0].cur / 10, frame[0].set_cur / 10, frame[0].rota);
                    break;
                }
            }
        }
        else
        {
            SALOG_ERROR("robomas", "recv failed.");
        }
        SATHREAD_YIELD();
    }
}


void RMM3508_safetyThread(void)
{
    int sock = SASocket_socket(AF_INET, SOCK_DGRAM, 0);

    size_t gw_count = 0;

    SALOCKER_LOCK(&g_obj.locker);
    gw_count = g_obj.gw_count;

    struct sockaddr_in safety_send_addr[4];
    for(size_t gw_i = 0; gw_i < gw_count; gw_i++)
    {
        safety_send_addr[gw_i].sin_family = AF_INET;
        safety_send_addr[gw_i].sin_port = htons(ROBOMAS_MOTOR_CONF_SEND_PORT);
        safety_send_addr[gw_i].sin_addr.s_addr = inet_addr(RMM3508_createIp(gw_i));
    }
    SALOCKER_UNLOCK(&g_obj.locker);

    while(1)
    {
        uint8_t safety;
        if(CCBool_get(g_obj.is_safety) == SABOOL_TRUE)
        {
            safety = 1;
        }else{
            safety = 0;
        }

        for(size_t gw_i = 0; gw_i < gw_count; gw_i++)
        {
            // SALOG_INFO("robomas", "gw_i:%zu, safety: %d", gw_i, safety);
            SASocket_sendto(sock, &safety, sizeof(uint8_t), 0, (struct sockaddr*)&safety_send_addr[gw_i], sizeof(struct sockaddr_in));
        }
        
        SATime_delayMs(200);
    }
}
