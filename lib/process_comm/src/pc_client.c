// #include "pc_client.h"

// #include "pc_type.h"

// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include "pc_mdns.h"


// void PCClient_init(void)
// {
// }


// CC_obj PCClient_get(const char* server_name, const char* path)
// {
//     int sock = PCMdns_createSocket(PCMdns_protocol_Udp, NULL);

//     struct timeval tv;
//     tv.tv_sec = 0;
//     tv.tv_usec = 500000;
//     SASOCKET_SETSOCKOPT(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

//     CC_obj recv_obj = NULL;

//     PCMdns_server_t server_addr = PCMdns_browseServer("pc_server", server_name);
//     if(server_addr.is_setted)
//     {
//         CCAutoRelease_startScope();
//         {
//             CC_obj send_obj = CCAutoRelease_add(CCDictionary_create());
//             CCDictionary_setObject(send_obj, CCString_create("GET"), "type");
//             CCDictionary_setObject(send_obj, CCString_create(path), "path");
//             CC_obj send_js = CCAutoRelease_add(CCJsonSerializer_dump(send_obj, SABOOL_FALSE));

// #ifdef PC_TYPE_LOG_ON
//             SALOG_INFO("process_comm", "%s", CCBinary_getRaw(send_js));
// #endif /*PC_TYPE_LOG_ON*/

//             PCMdns_sendtoServer(sock, server_addr, CCBinary_getRaw(send_js), CCBinary_size(send_js));

//             char buff[4096];
//             ssize_t length = SASOCKET_RECV(sock, buff, 4096, 0);
//             if(0 < length)
//             {
// #ifdef PC_TYPE_LOG_ON
//                 SALOG_INFO("process_comm", "%s", buff);
// #endif /*PC_TYPE_LOG_ON*/
//                 CC_obj recv_js = CCAutoRelease_add(CCBinary_createWithLength(buff, length));
//                 recv_obj = CCJsonSerializer_dynamicLoad(recv_js);
//             }
//         }
//         CCAutoRelease_doneScope();
//     }else{
//         SALOG_ERROR("process_comm", "Server not foound!");
//     }

//     SASOCKET_CLOSE(sock);

//     return recv_obj;
// }


// void PCClient_post(const char* server_name, const char* path, CC_obj set_obj)
// {
//     int sock = PCMdns_createSocket(PCMdns_protocol_Udp, NULL);

//     struct timeval tv;
//     tv.tv_sec = 0;
//     tv.tv_usec = 500000;
//     SASOCKET_SETSOCKOPT(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

//     PCMdns_server_t server_addr = PCMdns_browseServer("pc_server", server_name);
//     if(server_addr.is_setted)
//     {
//         CCAutoRelease_startScope();
//         {
//             CC_obj send_obj = CCAutoRelease_add(CCDictionary_create());
//             CCDictionary_setObject(send_obj, CCString_create("POST"), "type");
//             CCDictionary_setObject(send_obj, CCString_create(path), "path");
//             CCDictionary_setObject(send_obj, set_obj, "json");
//             CC_obj send_js = CCAutoRelease_add(CCJsonSerializer_dump(send_obj, SABOOL_FALSE));

//             PCMdns_sendtoServer(sock, server_addr, CCBinary_getRaw(send_js), CCBinary_size(send_js));
//         }
//         CCAutoRelease_doneScope();
//     }else{
//         SALOG_ERROR("process_comm", "Server not foound!");
//     }

//     SASOCKET_CLOSE(sock);
// }