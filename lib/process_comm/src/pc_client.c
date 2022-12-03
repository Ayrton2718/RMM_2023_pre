// #include "pc_client.h"

// #include "pc_type.h"

// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>


// struct PCClient_t
// {    
//     pthread_mutex_t locker;
//     int sock;
// };

// static struct PCClient_t g_obj;

// void PCClient_init(void)
// {
//     SALOCKER_INIT(&g_obj.locker, NULL);
//     g_obj.sock = SASocket_portScan(NULL);

//     struct timeval tv;
// #ifdef __APPLE__
//     tv.tv_sec = 10;
//     tv.tv_usec = 0;
// #else
//     tv.tv_sec = 0;
//     tv.tv_usec = 500000;
// #endif /*__APPLE__*/
//     SASocket_setsockopt(g_obj.sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
// }


// CC_obj PCClient_get(const char* server_name, const char* path)
// {
//     CC_obj recv_obj = NULL;

//     struct sockaddr_in addr;
//     if(PCDnsClient_getSockAddr(server_name, &addr) == SABOOL_TRUE)
//     {
//         CCAutoRelease_startScope();
//         {
//             CC_obj send_obj = CCAutoRelease_add(CCDictionary_create());
//             CCDictionary_setObject(send_obj, CCString_create("GET"), "type");
//             CCDictionary_setObject(send_obj, CCString_create(path), "path");
//             CC_obj send_js = CCJsonSerializer_dump(send_obj, SABOOL_FALSE);

// #ifdef PC_TYPE_LOG_ON
//             CCLOG_INFO("%s", CCString_getCString(send_js));
// #endif /*PC_TYPE_LOG_ON*/

//             SALOCKER_LOCK(&g_obj.locker);
//             SASocket_sendto(g_obj.sock, CCString_getCString(send_js), CCString_length(send_js) + 1, 0, (struct sockaddr*)&addr, sizeof(addr));

//             char buff[4096];
//             if(0 < SASocket_recv(g_obj.sock, buff, 4096, 0))
//             {
// #ifdef PC_TYPE_LOG_ON
//                 CCLOG_INFO("%s", buff);
// #endif /*PC_TYPE_LOG_ON*/
//                 CC_obj recv_js = CCAutoRelease_add(CCString_create(buff));
//                 recv_obj = CCJsonSerializer_dynamicLoad(recv_js);
//             }
//             SALOCKER_UNLOCK(&g_obj.locker);
//         }
//         CCAutoRelease_doneScope();
//     }else{
//         SALOG_ERROR("process_comm", "Server not foound!");
//     }

//     return recv_obj;
// }


// void PCClient_post(const char* server_name, const char* path, CC_obj set_obj)
// {
//     struct sockaddr_in addr;
//     if(PCDnsClient_getSockAddr(server_name, &addr) == SABOOL_TRUE)
//     {
//         CCAutoRelease_startScope();
//         {
//             CC_obj send_obj = CCAutoRelease_add(CCDictionary_create());
//             CCDictionary_setObject(send_obj, CCString_create("POST"), "type");
//             CCDictionary_setObject(send_obj, CCString_create(path), "path");
//             CCDictionary_setObject(send_obj, set_obj, "json");
//             CC_obj send_js = CCJsonSerializer_dump(send_obj, SABOOL_FALSE);

//             SALOCKER_LOCK(&g_obj.locker);
//             SASocket_sendto(g_obj.sock, CCString_getCString(send_js), CCString_length(send_js) + 1, 0, (struct sockaddr*)&addr, sizeof(addr));
//             SALOCKER_UNLOCK(&g_obj.locker);
//         }
//         CCAutoRelease_doneScope();
//     }else{
//         SALOG_ERROR("process_comm", "Server not foound!");
//     }
// }