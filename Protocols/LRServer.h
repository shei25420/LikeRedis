//
// Created by shei on 9/14/23.
//

#ifndef LIKEREDIS_LRSERVER_H
#define LIKEREDIS_LRSERVER_H

#include <cstdint>
#include <cstdio>
#include <vector>
#include "TCP/TCPServer.h"
#include "../System/EventLoop.h"
#include <unordered_map>

#define K_MAX_LEN 4096

enum {
    STATE_REQ = 0,
    STATE_RES = 1,
    STATE_END = 2
};
struct Conn {
    int sockFd;
    uint32_t state = STATE_REQ;
    size_t rbuf_size = 0;
    uint8_t rbuf[4 + K_MAX_LEN];

    size_t wbuf_size = 0;
    size_t wbuf_sent = 0;
    uint8_t wbuf[4 + K_MAX_LEN];
};

class LRServer: private TCPServer, EventLoop {
private:
    //Initialize Peer Connections Map
    std::unordered_map<int, Conn*> peerConnections;

    void handle_peer_connections();
    bool handle_one_request(Conn* conn);
    bool handle_write_buffer(Conn* conn);
    bool flush_write_buffer(Conn* conn);
    int handle_read_buffer(int peerFd);
public:
    LRServer(int port, int maxConn);
    void initializeServer();
};

#endif //LIKEREDIS_LRSERVER_H
