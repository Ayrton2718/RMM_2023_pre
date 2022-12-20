#ifndef STD_API_SRC_SA_SOCKET_H
#define STD_API_SRC_SA_SOCKET_H

#include "system/sa_type.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 

#ifdef __cplusplus
extern "C" {
#endif

int SASocket_socket(int socket_family, int socket_type, int protocol);
void SASocket_close(int fd);

int SASocket_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int SASocket_bindAnyPort(int socket_family, int socket_type, int protocol);

void SASocket_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);

size_t SASocket_recv(int sockfd, void *buf, size_t len, int flags);
size_t SASocket_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);

void SASocket_setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);


#ifdef __cplusplus
}
#endif

#endif /*STD_API_SRC_SA_SOCKET_H*/