//
// Created by shei on 9/13/23.
//

#ifndef LIKEREDIS_EVENTLOOP_H
#define LIKEREDIS_EVENTLOOP_H

#include <sys/epoll.h>

class EventLoop {
public:
    EventLoop();

    int evl_fd;
    int maxEvents = 40;
    struct epoll_event* events = nullptr;

    bool add_fd(int fd, struct epoll_event *event);
    bool modify_fd(int fd, struct epoll_event *event);
};

#endif //LIKEREDIS_EVENTLOOP_H
