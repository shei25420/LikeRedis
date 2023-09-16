//
// Created by shei on 9/13/23.
//

#include <cstring>
#include "Protocols/Helpers/TCPHelpers.h"
#include "Protocols/LRClient.h"

int main () {
    try {
        auto *client = new LRClient("127.0.0.1", "1234");

        //Pipeline
        char *query_list[3] = { "hello", "hello1", "hello2" };
        for (char* query : query_list) {
            size_t bytesSent = client->query(query);
            if (bytesSent < 0) return -1;
            printf("[debug] Sent %d bytes to server\n");
        }

        for (int i = 0; i < 3; ++i) {
            size_t bytesRead = 0;
            void* buffer = client->read_response(&bytesRead);
            if (!buffer) {
                return -1;
            }
            printf("Message echoed: %.*s\n", (int)bytesRead, (char *)buffer);
            client->free_read_response(buffer);
        }
    } catch (char* error) {
        fprintf(stderr, "[-] error running likeredis client\n");
        return -1;
    }
    return 0;
}
