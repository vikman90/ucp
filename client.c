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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>

#define PORT 1516
#define LENGTH 512
#define ATTEMPTS 4
#define DELAY 1

#define error(format, ...) fprintf(stderr, "ERROR: " format "\n", ##__VA_ARGS__)

#ifdef DEBUG
    #define debug(format, ...) fprintf(stderr, "DEBUG: " format "\n", ##__VA_ARGS__)
#else
    #define debug(format, ...)
#endif

int main(int argc, char ** argv) {
    int sock;
    int i;
    char c;
    size_t length;
    char *target = NULL;
    char buffer[LENGTH];
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = PORT, .sin_addr = { htonl(INADDR_LOOPBACK) } };
    FILE *fp;
    fd_set fdset;
    struct timeval timeout = { 0, 0 };

    // Get options

    while (c = getopt(argc, argv, "m:"), c != -1) {
        switch (c) {
        case 'm':
            target = optarg;
            break;

        case '?':
            if (optopt == 'c') {
                error("-%c needs an argument.", optopt);
            }

        default:
            fprintf(stderr, "Syntax: %s [ -m <addr> ] <file>\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (optind >= argc) {
        error("File not specified.");
        fprintf(stderr, "Syntax: %s [ -m <addr> ] <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open file

    if (fp = fopen(argv[optind], "r"), !fp) {
        perror("fopen()");
        return EXIT_FAILURE;
    }

    // Get address by name or IP

    if (target && !inet_aton(target, &addr.sin_addr)) {
        struct hostent *hostent;

        if (hostent = gethostbyname(target), !hostent) {
            error("Invalid target '%s'.", target);
            return EXIT_FAILURE;
        }

        addr.sin_addr = *(struct in_addr *)hostent->h_addr;
    }

    // Create socket

    if (sock = socket(PF_INET, SOCK_DGRAM, 0), sock < 0) {
        perror("socket()");
        return EXIT_FAILURE;
    }

    // Connect socket (set up a default destination)

    if (connect(sock, (const struct sockaddr *)&addr, sizeof(addr)), sock < 0) {
        perror("connect()");
        return EXIT_FAILURE;
    }

    // Send file content

    while (length = fread(buffer, sizeof(char), LENGTH, fp), length > 0) {
        for (i = 0; i < ATTEMPTS; i++) {
            // Send fragment

            if (send(sock, buffer, length, 0) < 0) {
                perror("send()");
                return EXIT_FAILURE;
            }

            // Wait for ACK

            FD_ZERO(&fdset);
            FD_SET(sock, &fdset);
            timeout.tv_sec = DELAY;
            timeout.tv_usec = 0;

            switch (select(sock + 1, &fdset, NULL, NULL, &timeout)) {
            case -1:
                perror("select()");
                continue;

            case 0:
                debug("Packet lost, resending.");
                continue;
            }

            if (recv(sock, buffer, LENGTH, 0) < 0) {
                perror("recv()");
                return EXIT_FAILURE;
            }

            break;
        }

        if (i == ATTEMPTS) {
            error("Maximum attempts exceeded.");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
