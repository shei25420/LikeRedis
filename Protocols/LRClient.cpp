//
// Created by shei on 9/15/23.
//

#include <cstdint>
#include <cstring>
#include "LRClient.h"
#include "Helpers/TCPHelpers.h"

LRClient::LRClient(char *hostname, char *port) : TCPClient(hostname, port) {

}

LRClient::~LRClient() {

}

size_t LRClient::query(char *text) {
    auto len = (uint32_t) strlen(text);
    if (len > K_MAX_LEN) {
        fprintf(stderr, "[-] message too long to send\n");
        return -1;
    }
    char w_buf[4 + K_MAX_LEN + 1] = { 0 };
    memcpy(w_buf, &len, 4);
    memcpy(&w_buf[4], text, len);;

    return TCPHelpers::send_data(cliSock, w_buf, 4 + len);
}

void *LRClient::read_response(size_t *bytesRead) {
    return TCPHelpers::recv_data(cliSock, bytesRead);
}

void LRClient::free_read_response(void *response) {
    return TCPHelpers::free_recv_data(response);
}
