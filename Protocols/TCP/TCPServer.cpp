//
// Created by shei on 9/13/23.
//

#include <sys/socket.h>
#include <cstdio>
#include <netinet/in.h>
#include <csignal>
#include "TCPServer.h"

TCPServer::TCPServer(int port, int maxConn) {
    printf("[+] initializing tcp socket...\n");
    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        perror("[-] error creating network socket");
        throw "Error initializing tcp server";
    }
    printf("[+] done initializing tcp socket...\n");

    //Reuse address in case of crash or restart
    int reuse = 1;
    if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse))) {
        perror("[-] error setting socket to reuse address");
        close(sockFd);
        throw "Error initializing tcp server";
    }

    printf("[+] attempting to bind to loopback address with port: %d\n", port);

    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockFd, (struct sockaddr*)&addr, sizeof(addr))) {
        perror("[-] error binding to interface");
        close(sockFd);
        throw "Error initializing tcp server";
    }

    printf("[+] listening for connections...\n");
    if (listen(sockFd, maxConn)) {
        perror("[-] error listening for connections");
        close(sockFd);
        throw "Error initializing tcp server";
    }
}
