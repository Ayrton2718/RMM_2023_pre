// #include "pc_server.h"

// #include <stdio.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <unistd.h>

// #include "pc_type.h"
// #include "pc_mdns.h"

// // // school wifi
// // #define PC_SERVER_MY_IP_ADDR    ("192.168.12.26")
// // #define PC_SERVER_MY_PORT_ADDR  (50005)
// // #define PC_SERVER_WS_IP_ADDR    ("192.168.12.162")
// // #define PC_SERVER_WS_PORT_ADDR  (9000)



// void PCServer_server(void);

// void PCServer_init(void)
// {
//     TCLauncher_launch(PCServer_server, "process_comm_server");
// }


// void PCServer_server(void)
// {
//     struct sockaddr_in addr;
//     char name[64];

//     CCAutoRelease_startScope();
//     {
//         CC_obj server_name = CCAutoRelease_add(TCDirectory_openObj("process_comm/my_servers", "name"));
//         size_t name_len = CCString_length(server_name);
//         if(63 < name_len)
//         {
//             name_len = 63;
//         }
//         memcpy(name, CCString_getCString(server_name), sizeof(char) * name_len);
//         name[name_len] = '\0';
//     }
//     CCAutoRelease_doneScope();

//     int sock = PCMdns_createSocket(PCMdns_protocol_Udp, &addr);
//     SALOG_INFO("process_comm", "pc_server start %s(%d)", name, ntohs(addr.sin_port));
//     // PCMdns_register_t server_reg = PCMdns_registServer(sock, &addr, PCMdns_protocol_Udp, "pc_server", name, NULL);

//     char buff[2048];
//     memset(buff, 0, sizeof(buff));

// 	struct sockaddr_in client_address;
// 	socklen_t client_address_len = sizeof(client_address);

//     while(1)
//     {
//         ssize_t recv_len = SASocket_recvfrom(sock, buff, sizeof(buff), 0, (struct sockaddr *)&client_address, &client_address_len);
//         if(0 < recv_len)
//         {
//             buff[recv_len] = '\0';

// #ifdef PC_TYPE_LOG_ON
//             CCLOG_INFO("received: '%s' from client %s[%d]", buff, inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
// #endif /*PC_TYPE_LOG_ON*/

//             CCAutoRelease_startScope();
//             {
//                 CC_obj js_string = CCAutoRelease_add(CCBinary_createWithLength(buff, recv_len));
//                 CC_obj order = CCAutoRelease_add(CCJsonSerializer_dynamicLoad(js_string));
//                 if(CCObject_isObject(order))
//                 {
//                     CC_obj type_string = CCAutoRelease_add(CCDictionary_objectForKey(order, "type"));
//                     CC_obj path_string = CCAutoRelease_add(CCDictionary_objectForKey(order, "path"));
//                     if(CCObject_isObject(type_string) && CCObject_isObject(path_string))
//                     {
//                         if(strncmp(CCString_getCString(type_string), "POST", strlen("POST")) == 0)
//                         {
//                             CC_obj post_obj = CCAutoRelease_add(CCDictionary_objectForKey(order, "json"));
//                             CC_obj posted_obj = TCDirectory_openObjAbsPath(CCString_getCString(path_string));

// // #ifdef PC_TYPE_LOG_ON
// //                             CCLOG_INFO_NOFMT(CCString_getCString(CCAutoRelease_add(CCJsonSerializer_dumps(post_obj, SABOOL_TRUE))));
// //                             CCLOG_INFO_NOFMT(CCString_getCString(CCAutoRelease_add(CCJsonSerializer_dumps(posted_obj, SABOOL_TRUE))));
// // #endif /*PC_TYPE_LOG_ON*/
//                             if(CCObject_replace(posted_obj, post_obj) == SABOOL_FALSE)
//                             {
//                                 SALOG_ERROR("process_comm", "post failed! \"%s\"", buff);
//                             }
//                         }
//                         else if(strncmp(CCString_getCString(type_string), "GET", strlen("GET")) == 0)
//                         {
//                             CC_obj res_js = CCAutoRelease_add(TCDirectory_lsRaw(CCString_getCString(path_string)));
//                             if(0 < CCBinary_size(res_js))
//                             {
// #ifdef PC_TYPE_LOG_ON
//                                 CCLOG_INFO_NOFMT(CCString_getCString(res_js));
// #endif /*PC_TYPE_LOG_ON*/
//                                 SASocket_sendto(sock, CCBinary_getRaw(res_js), CCBinary_size(res_js) + 1, 0, (struct sockaddr *)&client_address,  sizeof(client_address));
//                             }
//                         }
//                     }else{
//                         SALOG_ERROR("process_comm", "recv json is broken! \"%s\"", buff);
//                     } 
//                 }
//             }
//             CCAutoRelease_doneScope();
//         }
//     }

//     SASocket_close(sock);
// }
