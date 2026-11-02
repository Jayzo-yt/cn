// Checksum and CRC Error Detection - Sender
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
    int sock;
    struct sockaddr_in addr;
    char text[500], data_bits[100], padded[150], codeword[150], packet[800];
    char divisor[] = "1101";                 // x^3 + x^2 + 1
    char choice;

    printf("Enter message: ");
    fgets(text, sizeof(text), stdin);
    text[strcspn(text, "\n")] = 0;

    printf("Enter data bits for CRC (e.g. 100100): ");
    scanf("%s", data_bits);

    unsigned int chk = checksum(text);

    // CRC: append (n-1) zeros, divide, put the remainder after the data
    int n = strlen(divisor);
    strcpy(padded, data_bits);
    for (int i = 0; i < n - 1; i++)
        strcat(padded, "0");
    crc_remainder(padded, divisor);

    strcpy(codeword, data_bits);
    strcat(codeword, padded + strlen(data_bits));       // remainder bits

    printf("Checksum: %u\n", chk);
    printf("CRC remainder: %s | Codeword: %s\n", padded + strlen(data_bits), codeword);

    // Optional: introduce an error to test detection
    printf("Corrupt the data? (y/n): ");
    scanf(" %c", &choice);
    if (choice == 'y') {
        text[0] = 'X';                                  // corrupt the text
        codeword[0] = (codeword[0] == '1') ? '0' : '1'; // flip first bit
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000);
    connect(sock, (struct sockaddr *)&addr, sizeof(addr));

    sprintf(packet, "%s|%u|%s", text, chk, codeword);
    send(sock, packet, strlen(packet), 0);

    close(sock);
    return 0;
}
