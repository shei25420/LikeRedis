//
// Created by shei on 9/15/23.
//

#ifndef LIKEREDIS_LRCLIENT_H
#define LIKEREDIS_LRCLIENT_H

#define K_MAX_LEN 4096

#include "TCP/TCPClient.h"
#include "Helpers/TCPHelpers.h"
class LRClient : TCPClient {
public:
    LRClient(char *hostname, char *port);
    ~LRClient() override;
    size_t query(char* text);
    void* read_response(size_t* bytesRead);
    void free_read_response(void* response);
};


#endif //LIKEREDIS_LRCLIENT_H
