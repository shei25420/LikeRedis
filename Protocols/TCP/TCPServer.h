//
// Created by shei on 9/13/23.
//

#ifndef LIKEREDIS_TCPSERVER_H
#define LIKEREDIS_TCPSERVER_H

class TCPServer {
public:
    int sockFd;
    TCPServer(int port, int maxConn);
};


#endif //LIKEREDIS_TCPSERVER_H
