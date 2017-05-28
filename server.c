/* UDP File Copy
 * by Vikman
 * May 28, 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 1516
#define LENGTH 512

#define error(format, ...) fprintf(stderr, "ERROR: " format "\n", ##__VA_ARGS__)

#ifdef DEBUG
    #define debug(format, ...) fprintf(stderr, "DEBUG: " format "\n", ##__VA_ARGS__)
#else
    #define debug(format, ...)
#endif

int main() {
    int sock;
    ssize_t length;
    socklen_t socklen;
    char buffer[LENGTH];
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = PORT, .sin_addr = { htonl(INADDR_ANY) } };

    if (sock = socket(PF_INET, SOCK_DGRAM, 0), sock < 0) {
        perror("socket()");
        return EXIT_FAILURE;
    }

    if (bind(sock, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind()");
        return EXIT_FAILURE;
    }

    socklen = sizeof(length);

    if (getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &length, &socklen) < 0) {
        perror("getsockopt(SO_RCVBUF)");
        return EXIT_FAILURE;
    }

    socklen = sizeof(addr);
    printf("Waiting for connections. Buffer size = %zd bytes.\n", length);

    while (1) {
        switch (length = recvfrom(sock, buffer, LENGTH, 0, (struct sockaddr *)&addr, &socklen), length) {
        case -1:
            perror("recv()");
            return EXIT_FAILURE;

        case 0:
            printf("No data.\n");
            continue;
        }

        printf("[%zd]: %.*s", length, (int)length, buffer);
        fflush(stdout);

        // Send ACK

        if (sendto(sock, "", 0, 0, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("send(ACK)");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
