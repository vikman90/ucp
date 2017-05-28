/* UDP File Copy
 * by Vikman
 * May 28, 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#define SOCKET "ucp.sock"
#define LENGTH 512

int main(int argc, char ** argv) {
    int sock;
    size_t length;
    char buffer[LENGTH];
    struct sockaddr_un addr = { 0, AF_LOCAL, SOCKET };
    FILE *fp;

    if (argc < 2) {
        fprintf(stderr, "Syntax: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (fp = fopen(argv[1], "r"), !fp) {
        perror("fopen()");
        return EXIT_FAILURE;
    }

    if (sock = socket(PF_LOCAL, SOCK_DGRAM, 0), sock < 0) {
        perror("socket()");
        return EXIT_FAILURE;
    }

    if (connect(sock, (const struct sockaddr *)&addr, SUN_LEN(&addr)), sock < 0) {
        perror("connect()");
        return EXIT_FAILURE;
    }

    while (length = fread(buffer, sizeof(char), LENGTH, fp), length > 0) {
        if (send(sock, buffer, length, 0) < 0) {
            if (errno == ENOBUFS) {
                usleep(1000);
            } else {
                perror("send()");
                return EXIT_FAILURE;
            }
        }
    }

    return EXIT_SUCCESS;
}
