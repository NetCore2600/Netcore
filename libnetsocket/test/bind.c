
#include "netsocket.h"
#include <stdio.h>
#include <unistd.h>

int main() {

    pid_t pid = getpid();
    printf("Running PID is %d\n", pid);

    int sockfd = net_socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("my_socket");
        return 1;
    }

    net_bind(sockfd, "127.0.0.1", 8080);
    struct sockaddr_in socket = net_getsockname(sockfd);
    printf("port: %d\n", ntohs(socket.sin_port));

    while(1) {}

    close(sockfd);
    return 0;
}

