#include "sa_socket.h"

#include "system/sa_sys_log.h"
#include <netinet/in.h>
#include <unistd.h>


int SASocket_socket(int socket_family, int socket_type, int protocol)
{
    int sock = socket(socket_family, socket_type, protocol);
    if(sock < 0)
    {
        SASYS_LOG_ERROR_ERRNO("socket failed", sock);
        return -1;
    }
    return sock;
}


int SASocket_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int res = bind(sockfd, addr, addrlen);
    if(res < 0)
    {
        SASYS_LOG_ERROR_ERRNO("bind failed", res);
    }
    return res;
}


int SASocket_bindAnyPort(int socket_family, int socket_type, int protocol)
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    int sock = SASocket_socket(socket_family, socket_type, protocol);
    if(0 < sock)
    {
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(0);

        SASocket_bind(sock, (const struct sockaddr*)&addr, addr_len);
        return sock;
    }
    return -1;
}


void SASocket_close(int fd)
{
    int res = close(fd);
    if(res < 0)
    {
        SASYS_LOG_ERROR_ERRNO("close failed", res);
    }
}


void SASocket_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{
    ssize_t send_len = sendto(sockfd, buf, len, flags, dest_addr, addrlen);
    if(send_len != len)
    {
        SASYS_LOG_ERROR_ERRNO("sendto failed", send_len);
    }
}


size_t SASocket_recv(int sockfd, void *buf, size_t len, int flags)
{
    ssize_t res = recv(sockfd, buf, len, flags);
    if(res < 0)
    {
        SASYS_LOG_ERROR_ERRNO("recv failed", res);
        return 0;
    }
    return (size_t)res;
}

size_t SASocket_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)
{
    ssize_t res = recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
    if(res < 0)
    {
        SASYS_LOG_ERROR_ERRNO("recvfrom failed", res);
        return 0;
    }
    return (size_t)res;
}


void SASocket_setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
    if(setsockopt(sockfd, level, optname, optval, optlen) < 0)
    {
        SASYS_LOG_ERROR_ERRNO("setsockopt failed", -1);
    }
}
