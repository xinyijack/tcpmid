//
// Created by cryin on 2023/9/18.
//
#include "acceptot.h"
struct acceptor *init(int port) {
    struct acceptor *accept = malloc(sizeof(struct acceptor));
    accept->listen_port = port;
    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    make_nonblocking(listen_socket);
    struct sockaddr_in server_addr;
    socklen_t server_addr_len = sizeof(server_addr);
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;
    setsockopt(accept->listen_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    int rt1 = bind(listen_socket, (struct sockaddr *)&server_addr, server_addr_len);
    if (rt1 < 0) {
        error(1, errno, "bind error");
    }
    int rt2 = listen(listen_socket, LISTENQ);
    if (rt2 < 0) {
        error(listen_socket, errno, "listen failed");
    }
    accept->listen_socket = listen_socket;
    return accept;
}