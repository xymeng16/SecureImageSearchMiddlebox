//
// Created by xiangyi on 7/8/19.
//
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include "network_io.h"

using namespace std;


int main(int argc, char **argv)
{
    if (argc != 3)
    {
        perror("Usage: ./main <local-ip-address> <port>");
        exit(1);
    }
    int server_sock = open_socket(argv[1], argv[2], SOCK_STREAM), conn;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = (socklen_t) sizeof(client_addr);
    // polling wait for the incoming connection
    // assuming there is no concurrent connection in the experimental period
    while (1)
    {
        if((conn = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len)) < 0)
        {
            perror("Error when accepting the client connection.");
            exit(1);
        }
        cout << "Client " << inet_ntoa(client_addr.sin_addr) << " connected." << endl;
        flush(cout); // accept will block the process
    }
    return 0;
}