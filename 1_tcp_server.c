// TCP Server
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_fd, conn_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(client_addr);
    char buffer[1024];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);      // TCP socket

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(5000);

    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, 1);
    printf("Server waiting for connection...\n");

    conn_fd = accept(server_fd, (struct sockaddr *)&client_addr, &len);
    printf("Client connected\n");

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int n = recv(conn_fd, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0 || strcmp(buffer, "bye") == 0)
            break;
        printf("Client: %s\n", buffer);

        char reply[1100];
        sprintf(reply, "Server received: %s", buffer);
        send(conn_fd, reply, strlen(reply), 0);
    }

    close(conn_fd);
    close(server_fd);
    return 0;
}
