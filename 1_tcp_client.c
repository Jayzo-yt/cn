// TCP Client
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[1024], msg[1024];

    sock = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(5000);

    connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

    while (1) {
        printf("Enter message (bye to quit): ");
        fgets(msg, sizeof(msg), stdin);
        msg[strcspn(msg, "\n")] = 0;                  // remove newline

        send(sock, msg, strlen(msg), 0);
        if (strcmp(msg, "bye") == 0)
            break;

        memset(buffer, 0, sizeof(buffer));
        recv(sock, buffer, sizeof(buffer) - 1, 0);
        printf("%s\n", buffer);
    }

    close(sock);
    return 0;
}
