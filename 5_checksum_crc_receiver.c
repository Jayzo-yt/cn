#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

unsigned int checksum(char *data) {
    unsigned int total = 0;
    for (int i = 0; data[i] != '\0'; i++)
        total += (unsigned char)data[i];
    while (total >> 16)
        total = (total & 0xFFFF) + (total >> 16);
    return ~total & 0xFFFF;
}

void crc_remainder(char *bits, char *divisor) {
    int n = strlen(divisor);
    int len = strlen(bits);
    for (int i = 0; i <= len - n; i++) {
        if (bits[i] == '1') {
            for (int j = 0; j < n; j++)
                bits[i + j] = (bits[i + j] == divisor[j]) ? '0' : '1';
        }
    }
}

int main() {
    int server_fd, conn_fd;
    struct sockaddr_in addr = {0};
    char buffer[1024];
    char divisor[] = "1101";

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000);
    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 1);
    printf("Receiver waiting...\n");
    conn_fd = accept(server_fd, NULL, NULL);

    memset(buffer, 0, sizeof(buffer));
    recv(conn_fd, buffer, sizeof(buffer) - 1, 0);

    char *text = strtok(buffer, "|");
    unsigned int chk = atoi(strtok(NULL, "|"));
    char *codeword = strtok(NULL, "|");

    printf("Received text: %s\n", text);

    if (checksum(text) == chk)
        printf("Checksum: NO error\n");
    else
        printf("Checksum: ERROR detected\n");

    crc_remainder(codeword, divisor);
    int error = 0;
    for (int i = 0; codeword[i] != '\0'; i++)
        if (codeword[i] == '1')
            error = 1;
    if (error)
        printf("CRC: ERROR detected\n");
    else
        printf("CRC: NO error\n");

    close(conn_fd);
    close(server_fd);
    return 0;
}
