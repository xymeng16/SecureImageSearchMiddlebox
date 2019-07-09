//
// Created by xymeng on 7/9/19.
//

#include "network_io.h"
#include <strings.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>


int open_socket(const char *addr, const char *port, const int sock_type)
{
    struct addrinfo hints, *results;

    bzero(&hints, sizeof(hints)); // Always good to clear garbage memory.
    hints.ai_family = AF_INET;
    hints.ai_socktype = sock_type; // TCP

    int status;

    if ((status = getaddrinfo(addr, port, &hints, &results)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s.\n", gai_strerror(status));
        exit(1);
    }

    int sock = socket(results->ai_family, results->ai_socktype, results->ai_protocol); // Create the socket
    if (bind(sock, results->ai_addr, sizeof(struct sockaddr)) < 0) // Bind the socket to the local address
    {
        perror("bind");
        exit(1);
    }

    if (sock_type == SOCK_DGRAM)
    {
        return sock; // No need to listen for UDP conn
    }

    if (listen(sock, MAX_CONN) < 0)
    {
        perror("listen error!");

        exit(1);
    }

    return sock;
}
