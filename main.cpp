//
// Created by xiangyi on 7/8/19.
//
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include "network_io.h"
#include "data_handler.h"


using namespace std;


int main(int argc, char **argv)
{
    if (argc != 3)
    {
        perror("Usage: ./main <local-ip-address> <port>");
        exit(1);
    }
    int server_sock = open_socket(argv[1], argv[2], SOCK_STREAM), conn;
    sockaddr_in client_addr;
    socklen_t client_addr_len = (socklen_t) sizeof(client_addr);
    // polling wait for the incoming connection
    // assuming there is no concurrent connection in the experimental period
    while (1)
    {
        if ((conn = accept(server_sock, (sockaddr *) &client_addr, &client_addr_len)) < 0)
        {
            perror("Error when accepting the client connection.");
            exit(1);
        }
        cout << "Client " << inet_ntoa(client_addr.sin_addr) << " connected." << endl;
        int length = 0, uploadNum = 0;
        byte *buf;

        recv(conn, &length, sizeof(int), MSG_WAITALL);
        recv(conn, &uploadNum, sizeof(int), MSG_WAITALL);

        length = ntohl(length);
        uploadNum = ntohl(uploadNum);

        cout << "Header: length=" << length << ", uploadNum=" << uploadNum << endl;

        buf = new byte[length - 2 * sizeof(int)];
        ssize_t len = recv(conn, buf, length - 2 * sizeof(int), MSG_WAITALL);
        cout << "Data length (w/o header): " << len << '\n' << endl;
        flush(cout); // accept call will block the process
        data_pack data;
        unpack(buf, uploadNum, &data, len);

    }
    return 0;
}