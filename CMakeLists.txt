cmake_minimum_required(VERSION 3.26)
project(tcpmid C)

set(CMAKE_C_STANDARD 11)

add_executable(tcpmid tcpclient.c
        tcpserver.c)
