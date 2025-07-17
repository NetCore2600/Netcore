#include <pthread.h>
#include <stdlib.h>
#include "netsocket.h"

void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);

    // You can now read/write to client_fd
    printf("Thread: handling client fd=%d\n", client_fd);

    // Example: simple response
    const char *msg = "Hello from server!\n";
    net_send(client_fd, msg, strlen(msg), 0);

    while (1) {}

    close(client_fd);
    return NULL;
}

int main() {

    pid_t pid = getpid();
    printf("Running on %d ...\n", pid);

    int server_fd = net_socket(AF_INET, SOCK_STREAM, 0);
    net_bind(server_fd, "127.0.0.1", 8080);
    net_listen(server_fd, 10);

    printf("Server listening on 127.0.0.1:8080...\n");

    while (1) {
        struct sockaddr_in client;
        uint32_t client_len = sizeof(client);

        int *client_fd = malloc(sizeof(int));
        *client_fd = net_accept(server_fd, (struct sockaddr_in *)&client, &client_len);
        if (*client_fd == -1) {
            free(client_fd);
            continue;
        }

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, client_fd);
        pthread_detach(tid); // so we don't need to call pthread_join
    }

    net_close(server_fd);
    return 0;
}
