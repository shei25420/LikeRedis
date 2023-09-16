//
// Created by shei on 9/13/23.
//

#include "Protocols/LRServer.h"

int main () {
    auto *server = new LRServer(1234, 10);
    server->initializeServer();
}