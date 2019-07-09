//
// Created by xymeng on 7/9/19.
//

#ifndef SECUREIMAGESEARCHMIDDLEBOX_NETWORK_IO_H
#define SECUREIMAGESEARCHMIDDLEBOX_NETWORK_IO_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#define MAX_CONN 16
#define SERVER_PORT "8888"

int open_socket(const char *addr, const char *port, const int sock_type);
void readn(int )
#endif //SECUREIMAGESEARCHMIDDLEBOX_NETWORK_IO_H
