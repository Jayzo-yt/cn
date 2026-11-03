#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server = {0};
    char msg[100], reply[120];

    server.sin_family = AF_INET;
    server.sin_port = htons(5000);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    bind(server_fd, (struct sockaddr *)&server, sizeof(server));
    listen(server_fd, 1);
    printf("Waiting for client...\n");

    int client = accept(server_fd, NULL, NULL);

    while (1) {
        int n = recv(client, msg, sizeof(msg) - 1, 0);
        if (n <= 0)
            break;
        msg[n] = '\0';
        if (strcmp(msg, "bye") == 0)
            break;
        printf("Client: %s\n", msg);
        snprintf(reply, sizeof(reply), "Server received: %s", msg);
        send(client, reply, strlen(reply), 0);
    }

    close(client);
    close(server_fd);
    return 0;
}
