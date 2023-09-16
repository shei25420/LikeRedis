//
// Created by shei on 9/15/23.
//

#ifndef LIKEREDIS_TCPCLIENT_H
#define LIKEREDIS_TCPCLIENT_H
struct TCPClient {
    int cliSock;
    TCPClient(char *hostname, char *port);
    virtual ~TCPClient();
};
#endif //LIKEREDIS_TCPCLIENT_H
