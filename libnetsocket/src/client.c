#include <pthread.h>
#include <stdlib.h>
#include "netsocket.h"

void handle_recv(int socketfd) {
    char buffer[1024];
    ssize_t received;

    struct sockaddr_in src_addr;
    socklen_t addr_len = sizeof(src_addr);

    while ((received = net_recvfrom(socketfd, buffer, sizeof(buffer) - 1, 0,
                                    (struct sockaddr *)&src_addr, &addr_len)) > 0) {
        buffer[received] = '\0';  // Null-terminate the received data

        printf("Received from %s:%d => %s",
            inet_ntoa(src_addr.sin_addr),
            ntohs(src_addr.sin_port),
            buffer);
    }

    if (received == -1) {
        perror("net_recvfrom failed");
    } else {
        printf("Server closed the connection.\n");
    }
}

int main() {

    pid_t pid = getpid();
    printf("Running pid %d ...\n", pid);

    int client_fd = net_socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (net_connect(client_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("net_connect failed");
        return 1;
    }

    handle_recv(client_fd);

    net_close(client_fd);
    return 0;
}
