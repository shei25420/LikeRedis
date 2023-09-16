//
// Created by shei on 9/13/23.
//
#include <cstdio>
#include <cstdlib>
#include "EventLoop.h"

EventLoop::EventLoop() {
    evl_fd = epoll_create1(0);
    if (evl_fd == -1) {
        perror("[-] error creating event loop poll fd");
        throw "Failed initializing event loop";
    }

    //Initialize the events buffer
    events = (struct epoll_event*)calloc(maxEvents, sizeof(struct epoll_event));
    if (!events) {
        perror("Error allocating memory for event loop events buffer");
        throw "Failed initializing event loop";
    }
}

bool EventLoop::add_fd(int fd, struct epoll_event *event)  {
    if (epoll_ctl(evl_fd, EPOLL_CTL_ADD, fd, event)) {
        perror("[+] error adding descriptor to event loop");
        return false;
    }
    return true;
}

bool EventLoop::modify_fd(int fd, struct epoll_event *event) {
    if (epoll_ctl(evl_fd, EPOLL_CTL_MOD, fd, event)) {
        perror("[-] error modifying descriptor in event loop");
        return false;
    }
    return true;
}

