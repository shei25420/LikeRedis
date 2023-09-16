//
// Created by shei on 9/14/23.
//
#include <sys/socket.h>
#include <csignal>
#include <cerrno>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include "LRServer.h"
#include "Helpers/TCPHelpers.h"

LRServer::LRServer(int port, int maxConn) : TCPServer(port, maxConn), EventLoop() {
    //First Set TCP Server Socket to non-blocking
    if (!TCPHelpers::fd_set_nb(sockFd)) {
        throw "Error setting tcp server to non blocking";
    }

    //Add Server Socket to Event Loop
    struct epoll_event event = { 0 };
    event.data.fd = sockFd;
    event.events = EPOLLIN | EPOLLET;

    if(!add_fd(sockFd, &event)) {
        throw "Error adding tcp server to event loop";
    }
}

void LRServer::initializeServer() {
    //Start The Event Loop
    for (;;) {
        //Handle all active connections to check those that need to be closed
        for (auto conn: peerConnections) {
            if (conn.second->state == STATE_END) {
                //Close peer connection
                close(conn.second->sockFd);
            }
        }

        int nfds = epoll_wait(evl_fd, events, maxEvents, -1);
        if (nfds == -1) {
            perror("[-] error waiting to events");
            break;
        }

        //Loop through read file descriptors
        for (int i = 0; i < nfds; i++) {
            //Check if the ready descriptor is the tcp socket (incoming new connection)
            if (events[i].events & EPOLLERR) {
                perror("[-] something went wrong with the descriptor");
                close(events[i].data.fd);
                continue;
            } else if (events[i].data.fd == sockFd) {
                handle_peer_connections();
            } else if (events[i].events & EPOLLIN) {
                //Socket is ready for reading data;
                handle_read_buffer(events[i].data.fd);
            } else if (events[i].events & EPOLLOUT) {
                //Socket is ready for writing
                printf("[debug] fuckshit\n");
            }
        }
    }
}



void LRServer::handle_peer_connections() {
    struct sockaddr cliAddr = { 0 };
    socklen_t cliAddrLen = sizeof(cliAddr);

    int peerFd = accept(sockFd, &cliAddr, &cliAddrLen);
    if (peerFd == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) return;

        perror("[-] Error accepting new peer connection");
        return;
    }

    //Make Peer Socket to non-blocking
    if (!(TCPHelpers::fd_set_nb(peerFd))) {
        fprintf(stderr, "[-] error setting peer socket to non blocking\n");
        close(peerFd);
        return;
    }

    auto *conn = (struct Conn*)calloc(1, sizeof(struct Conn));
    if (!conn) {
        perror("[-] error allocating connection struct");
        close(peerFd);
    }

    conn->sockFd = peerFd;
    conn->state = STATE_REQ;
    peerConnections[conn->sockFd] = conn;

    //Add Peer Connection to Event Loop
    struct epoll_event event = { 0 };
    event.data.fd = peerFd;
    event.events = EPOLLIN;
    if (!(EventLoop::add_fd(peerFd, &event))) {
        fprintf(stderr, "[-] error adding peer socket to event loop\n");
        close(peerFd);
        return;
    }
}

int LRServer::handle_read_buffer(int peerFd) {
    Conn* conn = peerConnections[peerFd];
    assert(conn->rbuf_size < sizeof(conn->rbuf));

    size_t bytesRead = 0;
    do {
        errno = 0;
        size_t cap = sizeof(conn->rbuf) - conn->rbuf_size;
        bytesRead = read(conn->sockFd, ((unsigned char *)conn->rbuf) + conn->rbuf_size, cap);
        if (errno == EAGAIN) {
            //Read all Data
            conn->state = STATE_RES;
            assert(conn->rbuf_size <= (sizeof(conn->rbuf) - conn->rbuf_size));
            while(handle_one_request(conn)){};
            break;
        }
        conn->rbuf_size += bytesRead;
    } while ((bytesRead > 0));

     if (bytesRead == 0) {
        if (conn->rbuf_size == 0) {
            fprintf(stderr, "[-] unexpected end of file\n");
        }
        //E0F and peer has closed connection
        conn->state = STATE_END;
        return 0;
    }
    return (conn->state == STATE_REQ);
}

bool LRServer::handle_one_request(Conn *conn) {
    if (conn->rbuf_size < 4) {
        //Not enough Data in the buffer to parse
        return false;
    }

    uint32_t len = 0;
    memcpy(&len, &conn->rbuf[0], 4);
    if (len > K_MAX_LEN) {
        fprintf(stderr, "[-] message is too long\n");
        conn->state = STATE_END;
        return false;
    }

    if ((4 + len) > conn->rbuf_size) {
        //Haven't finished reading the whole message
        return false;
    }

    //Got one message, Process It
    printf("[+] client says %.*s\n", len, &conn->rbuf[4]);

    //Generate Echoing Message (test)
    memcpy(&conn->wbuf[0], &len, 4);
    memcpy(&conn->wbuf[4], &conn->rbuf[4], len);
    conn->wbuf_size = 4 + len;

    //Remove the request from the buffer
    //NB using memmove frequently is inefficient
    size_t remain = (conn->rbuf_size - 4 - len);
    if (remain) {
        memmove(conn->rbuf, &conn->rbuf[4 + len], remain);
    }
    conn->rbuf_size = remain;
    conn->state = STATE_RES;

    //Change Peer Descriptor Listening State
    epoll_event event = { 0 };
    event.data.fd = conn->sockFd;
    event.events = EPOLLOUT;
    if (!(modify_fd(conn->sockFd, &event))) {
        fprintf(stderr, "[-] error adding descriptor for writing events\n");
        conn->state = STATE_END;
        return false;
    }


    //Start Writing Back the message back
    handle_write_buffer(conn);
    return (conn->state == STATE_REQ);
}

bool LRServer::handle_write_buffer(Conn *conn) {
    while(flush_write_buffer(conn)) {}
    return false;
}

bool LRServer::flush_write_buffer(Conn *conn) {
    size_t bytesWritten = 0;
    do {
        size_t remaining = (conn->wbuf_size - conn->wbuf_sent);
        bytesWritten = write(conn->sockFd, &conn->wbuf[conn->wbuf_sent], remaining);
        if (bytesWritten ==  -1) break;

        conn->wbuf_sent += bytesWritten;
        assert(conn->wbuf_sent <= conn->wbuf_size);
        if (conn->wbuf_sent == conn->wbuf_size) {
            //Done writing buffer. Change Event
            conn->wbuf_sent = 0;
            conn->wbuf_size = 0;

            //Switch back to EPOLLIN ON descriptor
            struct epoll_event event = { 0 };
            event.data.fd = conn->sockFd;
            event.events = EPOLLIN | EPOLLET;
            if (!EventLoop::modify_fd(conn->sockFd, &event)) {
                fprintf(stderr, "[-] error switching peer socket to request state\n");
                conn->state = STATE_END;
            } else conn->state = STATE_REQ;

            return false;
        }
    } while (bytesWritten > 0);

    if (errno == EAGAIN) {
        //Go again
        return false;
    } else if (bytesWritten != 0) {
        perror("[-] error sending data to peer");
        conn->state = STATE_END;
        return false;
    }
    return true;
}