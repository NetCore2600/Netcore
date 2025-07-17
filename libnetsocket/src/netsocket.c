#define _GNU_SOURCE
#include "netsocket.h"

/*
 * param {domain} int:  
 * param {type} int:  
 * param {protocol} int:   
 */
int net_socket(int domain, int type, int protocol) {
    int netsocketfd = syscall(NET_socket, domain, type, protocol);
    return netsocketfd;
}



int net_bind(int sockfd, const char *_addr, u_int16_t _port)
{
    struct stat f_info;
    struct in_addr addr;
    struct sockaddr_in sa;

    if (fstat(sockfd, &f_info) == -1) {
        perror("fstat failed");
        return -1;
    }

    // Check that sockfd is a socket file
    // S_IFMT => bitmask for the file type
    // S_IFSOCK => socket file type
    // 0140000 (socket) | 0000777 (permissions)
    // = 0140777 (octal)
    // = 49663 (decimal)
    if ((f_info.st_mode & S_IFMT) != S_IFSOCK) {
        perror("Not a socket\n");
        return -1;
    }

    // Check if address is a valid IPV4 
    if ( inet_pton(AF_INET, _addr, &addr) == 0) {
        perror("Address not available\n");
        return -1;
    }


    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(_port);
    sa.sin_addr = addr;
    // Perform the bind syscall
    if (syscall(NET_bind, sockfd, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
        if (errno == EADDRINUSE) {
            fprintf(stderr, "Address or port already in use: %s:%u\n", _addr, _port);
        } else {
            perror("bind syscall failed");
        }
        return -1;
    }
    
    return 0;
}

struct sockaddr_in net_getsockname(int sockfd)
{
    struct sockaddr_in result;
    uint32_t len = sizeof(result);

    memset(&result, 0, sizeof(result));
    if (syscall(NET_getsockname, sockfd, (struct sockaddr *)&result, &len) == -1) {
        perror("syscall(NET_getsockname) failed");
        result.sin_family = -1;  // mark as invalid
    }

    return result;
}

int net_listen(int sockfd, int backlog) {
    int res = syscall(NET_listen, sockfd, backlog);
    if (res == -1) {
        perror("net_listen: syscall(NET_listen) failed");
    }
    return res;
}

int net_accept(int sockfd, struct sockaddr_in *addr, uint32_t *addrlen) {
    int client_fd = syscall(NET_accept, sockfd, addr, addrlen);

    if (client_fd == -1) {
        perror("net_accept: syscall(SYS_accept) failed");
    }

    return client_fd;
}

int net_connect(int sockfd, const struct sockaddr *addr, uint32_t addrlen) {
    int res = syscall(NET_connect, sockfd, addr, addrlen);
    if (res == -1) {
        perror("net_connect: syscall(NET_connect) failed");
    }
    return res;
}

ssize_t net_send(int sockfd, const void *buf, size_t len, int flags) {
    ssize_t sent = syscall(NET_sendto, sockfd, buf, len, flags, NULL, 0);
    if (sent == -1) {
        perror("net_send: syscall(SYS_sendto) failed");
    }
    return sent;
}

ssize_t net_recvmsg(int sockfd, struct msghdr *msg, int flags) {
    ssize_t received = syscall(NET_recvmsg, sockfd, msg, flags);
    if (received == -1) {
        perror("net_recvmsg: syscall failed");
    }
    return received;
}

ssize_t net_recvfrom(int sockfd, void *buf, size_t len, int flags,
                     struct sockaddr *src_addr, uint32_t *addrlen) {
    ssize_t received = syscall(NET_recvfrom, sockfd, buf, len, flags, src_addr, addrlen);
    if (received == -1) {
        perror("net_recvfrom: syscall failed");
    }
    return received;
}

int net_close(int sockfd) {
    int res = syscall(NET_close, sockfd);
    if (res == -1) {
        perror("net_close: syscall(NET_close) failed");
    }
    return res;
}
