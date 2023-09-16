//
// Created by shei on 9/13/23.
//

#ifndef LIKEREDIS_TCPHELPERS_H
#define LIKEREDIS_TCPHELPERS_H

#include <cstdio>

struct TCPHelpers {
    static size_t send_data(int sockFd, void* buffer, size_t bufferLen);
    static void* recv_data(int sockFd, size_t *bytesRead);
    static void free_recv_data(void* data);
    static bool fd_set_nb (int fd);

};

#endif //LIKEREDIS_TCPHELPERS_H
