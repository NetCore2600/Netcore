#ifndef NETSOCKET_H
#define NETSOCKET_H

#include <stdint.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

// syscall
// https://chromium.googlesource.com/chromiumos/docs/+/master/constants/syscalls.md
#define NET_close 3 
#define NET_socket 41
#define NET_connect 42
#define NET_accept 43
#define NET_sendto 44
#define NET_recvfrom 45 
#define NET_recvmsg 47
#define NET_bind 49
#define NET_listen 50
#define NET_getsockname 51

/**
 * This si the struct representation of a file from <sys/stat.h> lib
 *
 *  struct stat {
 *      dev_t     st_dev;     // ID of device containing file
 *      ino_t     st_ino;     // Inode number
 *      mode_t    st_mode;    // File type and mode (permissions)
 *      nlink_t   st_nlink;   // Number of hard links
 *      uid_t     st_uid;     // User ID of owner
 *      gid_t     st_gid;     // Group ID of owner
 *      dev_t     st_rdev;    // Device ID (if special file)
 *      off_t     st_size;    // Total size, in bytes
 *      blksize_t st_blksize; // Block size for filesystem I/O
 *      blkcnt_t  st_blocks;  // Number of 512B blocks allocated
 *      
 *      struct timespec st_atim;  // Last access time
 *      struct timespec st_mtim;  // Last modification time
 *      struct timespec st_ctim;  // Last status change time
 *  };
*/

/**
 * This is the typical IPv4 socket address structure from <netinet/in.h>
 *
 *  struct sockaddr_in {
 *      sa_family_t    sin_family; // Address family (AF_INET)
 *      in_port_t      sin_port;   // Port number (in network byte order)
 *      struct in_addr sin_addr;   // IP address
 *      char           sin_zero[8];// Padding (not used, reserved)
 *  };
 *
 *  struct in_addr {
 *      uint32_t s_addr;           // IPv4 address (in network byte order)
 *  };
 *
 *  This structure is cast to (struct sockaddr *) when passed to bind(), connect(), etc.
 */

int                 net_socket(int domain, int type, int protocol);
int                 net_bind(int sockfd, const char *address, u_int16_t port);
struct sockaddr_in  net_getsockname(int sockfd);
int                 net_listen(int sockfd, int backlog);
int                 net_accept(int sockfd, struct sockaddr_in *addr, uint32_t *addrlen);
int                 net_connect(int sockfd, const struct sockaddr *addr, uint32_t addrlen);
ssize_t             net_send(int sockfd, const void *buf, size_t len, int flags);
ssize_t             net_recvmsg(int sockfd, struct msghdr *msg, int flags);
ssize_t             net_recvfrom(int sockfd, void *buf, size_t len, int flags,
                     struct sockaddr *src_addr, uint32_t *addrlen);
int                 net_close(int sockfd);

#endif // NETSOCKET_H
