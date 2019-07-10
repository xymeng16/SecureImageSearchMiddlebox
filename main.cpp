//
// Created by xiangyi on 7/8/19.
//
#include <iostream>
#include <cstring>

#include "network_io.h"

using namespace std;


int main(int argc, char **argv)
{
    if (argc != 2)
    {
        perror("Usage: ./main <local-ip-address>");
        exit(1);
    }
    int server_sock = open_socket(argv[1], SERVER_PORT, SOCK_STREAM), conn;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = (socklen_t) sizeof(client_addr);
    // polling-waiting for the incoming connection
    while (1)
    {
        if((conn = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len)) < 0)
        {
            perror("Error when accepting the client connection.");
            exit(1);
        }
        cout << client_addr.sin_addr.s_addr;
        flush(cout);
    }
    return 0;
}