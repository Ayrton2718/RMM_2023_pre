#ifndef PC_TYPE_H
#define PC_TYPE_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

// #define PC_TYPE_LOG_ON	(1)

// #define PC_DNS_TYPE_DNS_SERVER_ADDR   ("127.0.0.1")
// #define PC_DNS_TYPE_DNS_SERVER_PORT   (61233)

// #define PC_TYPE_SERVER_PORT				(61233)

// 登場人物：
// rmmプロセス一つにつき、serverとclientそれぞれ一つずつが起動している
// ・server -- RobotRMMのサーバー、clientからの応答に答えて、jsonを返す・変更する
// ・client -- serverに"GET"や"POST"のリクエストを送る
// ・rmm_dns -- rmmのdnsサーバー

// ※typeは、数字にする予定やけど、まだ未定義

// serverは、ブロードキャスト(port:50100)で、自分のIP、rmm_name、ポートを送信
// 例）{"type":1, "rmm_name":"robot1", "server_addr":[192,168,12,9], "server_port":6040}
// rmm_dnsは、送り元に、自分のIPと自分のポート番号を知らせるとともに、
// 静的に割り振っている、相手のIPとポートを送信。
// 例）{
//        "type":2
//        "dns_addr": [192, 168, 12, 9],
//        "dhs_port": 6040,
//        "static_servers": [
//            {"rmm_name": "robot2", "server_addr":[192, 168, 12, 17], "server_port": 54839},
//            {"rmm_name": "robot6", "server_addr":[192, 168, 12, 74], "server_port": 73473}
//        ]
//    }
// 　rmm_dnsが複数の場合ー＞エラー。

// rmm_serverは、定期的にrmm_dnsに自分の生存パケットを送信。（4秒に１回）
// {"type":3, rmm_name":"robot1", "status":1}
// rmm_dnsは、15秒を超えて生存パケットが届かない相手は、リストから消去（静的に割り振られたサーバーは別

// clientは、データを送りたいor受け取りたい相手serverのIPとポートを
// rmm_nameからrmm_dnsに問い合わせる。
// {"type":4, rmm_name":"robot4"}
// rmm_dnsは、rmm_nameとIPアドレス、ポート番号を送り返す。
// {"type":5, rmm_name":"robot4",  "server_addr":[192, 168, 12, 18], "server_port": 748728}

typedef enum
{
	CSType_packetType_BootInfo_t = 1,
	CSType_packetType_DnsInfo_t = 2,
	CSType_packetType_ServerStatus_t = 3,
	CSType_packetType_ServerAsk_t = 4,
	CSType_packetType_ServerInfo_t = 5,
    CSType_packetType_Ack_t = 6,
    CSType_packetType_Ping_t = 7
} PCType_packetType_t;


#ifdef __cplusplus
}
#endif

#endif /*PC_TYPE_H*/