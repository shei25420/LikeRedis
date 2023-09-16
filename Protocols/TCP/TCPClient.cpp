//
// Created by shei on 9/15/23.
//

#include <sys/socket.h>
#include <cstdio>
#include <netdb.h>
#include <csignal>
#include "TCPClient.h"

TCPClient::TCPClient(char *hostname, char *port) {
    //Configure host to connect to
    printf("[+] configuring server address...\n");
    struct addrinfo hints = { 0 };
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo* addr = nullptr;
    int ret = getaddrinfo(hostname, port, &hints, &addr);
    if (ret) {
        fprintf(stderr, "[-] error resolving address: %s\n", gai_strerror(ret));
        throw "Errpr initializing TCP Client";
    }
    printf("[+] server configured successfully\n");
    printf("[+] initializing client socket...\n");

    cliSock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (cliSock < 0) {
        perror("[-] error creating client socket");
        freeaddrinfo(addr);
        throw "Errpr initializing TCP Client";
    }
    printf("[+] client socket initialized\n");

    printf("[+] attempting connection...\n");
    if (connect(cliSock, addr->ai_addr, addr->ai_addrlen)) {
        perror("[-] error connecting to host");
        freeaddrinfo(addr);
        throw "Errpr initializing TCP Client";
    }
    freeaddrinfo(addr);
    printf("[+] connection established successfully\n");
}

TCPClient::~TCPClient() {
    close(cliSock);
}
