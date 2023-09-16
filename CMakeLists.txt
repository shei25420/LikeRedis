cmake_minimum_required(VERSION 3.26)
project(LikeRedis)

set(CMAKE_CXX_STANDARD 17)

add_executable(LIkeRedisClient LIkeRedisClient.cpp
        Protocols/TCP/TCPClient.cpp
        Protocols/TCP/TCPClient.h
        Protocols/Helpers/TCPHelpers.cpp
        Protocols/Helpers/TCPHelpers.h
        Protocols/LRClient.cpp
        Protocols/LRClient.h
)
add_executable(LikeRedisServer LikeRedisServer.cpp
        Protocols/TCP/TCPServer.cpp
        Protocols/TCP/TCPServer.h
        System/EventLoop.cpp
        System/EventLoop.h
        Protocols/Helpers/TCPHelpers.cpp
        Protocols/Helpers/TCPHelpers.h
        Protocols/LRServer.cpp
        Protocols/LRServer.h
)
