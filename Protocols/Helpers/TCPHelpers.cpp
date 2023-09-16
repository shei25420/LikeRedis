//
// Created by shei on 9/13/23.
//

#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <cerrno>
#include <fcntl.h>
#include "TCPHelpers.h"


size_t TCPHelpers::send_data(int sockFd, void* buffer, size_t bufferLen) {
    auto * cursor = (u_int8_t*)buffer;
    size_t totalSent = 0;

    while (totalSent != bufferLen) {
        size_t bytesSent = send(sockFd, cursor, (bufferLen - totalSent), 0);
        if (bytesSent == -1) {
            perror("[-] error sending data");
            return bytesSent;
        }

        totalSent += bytesSent;
        cursor += bytesSent;
    }
    return totalSent;
}

void* TCPHelpers::recv_data(int sockFd, size_t *bytesRead) {
    int respLen = 1024;
    void* response = calloc(1, respLen);
    if (!response) {
        perror("[-] error allocating response memory");
        return nullptr;
    }

    void *cursor = response;
    void *end = ((unsigned char*)cursor) + respLen;
    while (true) {
        if (cursor == end) {
            //Save Current Cursor Offset
            long long cursorOffset = ((unsigned char*)cursor) - ((unsigned char*)response);

            //Reallocate buffer and double the size
            respLen *= 2;
            void* ret = realloc(response, respLen);
            if (!ret) {
                free(response);
                return nullptr;
            }
            cursor = ((unsigned char*)response) + cursorOffset;
        }

        size_t bytesRcvd = recv(sockFd, cursor, (((unsigned char*)end) - ((unsigned char*)cursor)), 0);
        if (bytesRcvd == 0) break;
        else if (bytesRcvd ==  -1) {
            perror("[-] error receiving data");
            free(response);
            return nullptr;
        }

        *bytesRead += bytesRcvd;
        cursor = ((unsigned char*)cursor) + bytesRcvd;
    }
    return response;
}

void TCPHelpers::free_recv_data(void* data) {
    free(data);
}

bool TCPHelpers::fd_set_nb (int fd) {
    errno = 0;
    int flags = fcntl(fd, F_GETFL, 0);
    if (errno) {
        perror("[-] fetching descriptor flags failed");
        return false;
    }

    flags |= O_NONBLOCK;
    errno = 0;

    (void) fcntl(fd, F_SETFL, flags);
    if (errno) {
        perror("[-] error setting non blocking flag to fd");
        return false;
    }
    return true;
}