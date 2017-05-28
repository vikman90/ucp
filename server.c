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
#include <sys/un.h>

#define SOCKET "ucp.sock"
#define LENGTH 512

int main() {
    int sock;
    ssize_t length;
    socklen_t socklen;
    char buffer[LENGTH];
    struct sockaddr_un addr = { 0, AF_LOCAL, SOCKET };

    if (sock = socket(PF_LOCAL, SOCK_DGRAM, 0), sock < 0) {
        perror("socket()");
        return EXIT_FAILURE;
    }

    unlink(SOCKET);

    if (bind(sock, (const struct sockaddr *)&addr, SUN_LEN(&addr)) < 0) {
        perror("bind()");
        return EXIT_FAILURE;
    }

    socklen = sizeof(length);

    if (getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &length, &socklen) < 0) {
        perror("getsockopt(SO_RCVBUF)");
        return EXIT_FAILURE;
    }

    printf("Waiting for connections. Buffer size = %zd bytes.\n", length);

    while (1) {
        switch (length = recv(sock, buffer, LENGTH, 0), length) {
        case -1:
            perror("recv()");
            return EXIT_FAILURE;

        case 0:
            printf("No data.\n");
            continue;
        }

        printf("[%zd]: %.*s", length, (int)length, buffer);
        fflush(stdout);
    }

    return EXIT_SUCCESS;
}
