//
// Created by jack on 23-8-29.
//
#include "lib/common.h"

#define MAXEVENTS 128

char rot13_char(char c) {
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
        return c - 13;
    else
        return c;
}

int main() {
    int listen_fd, socket_fd, efd, server_len, n;
    struct epoll_event event;
    struct epoll_event events[MAXEVENTS];

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    int on = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
    fcntl(listen_fd, F_SETFL, O_NONBLOCK);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERV_PORT);
    server_len = sizeof(server_addr);
    if (bind(listen_fd, (struct sockaddr *)&server_addr, server_len) < 0) {
        error(1, errno, "bind error");
    }

    if (listen(listen_fd, LISTENQ) < 0) {
        error(1, errno, "listen error");
    }

    signal(SIGPIPE, SIG_IGN);

    efd = epoll_create1(0);
    event.data.fd = listen_fd;
    event.events = EPOLLET | EPOLLIN;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, listen_fd, &event) == -1) {
        error(1, errno, "epoll_ctl error");
    }
    while(1) {
        n = epoll_wait(efd, events, MAXEVENTS, -1);
        for (int i = 0; i < n; i++) {
            if ( (events[i].events & EPOLLHUP) || (events[i].events & EPOLLERR) || !(events[i].events & EPOLLIN) ) {
                fprintf(stderr, "epoll error");
                close(events[i].data.fd);
                continue;
            } else if (events[i].events == listen_fd) {
                struct sockaddr_in client_addr;
                socklen_t sl = sizeof(client_addr);
                socket_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &sl);
                if (socket_fd < 0) {
                    error(1, errno, " accept error");
                } else {
                    event.data.fd = socket_fd;
                    event.events = EPOLLIN;
                    fcntl(socket_fd, F_SETFL, O_NONBLOCK);
                    if (epoll_ctl(efd, EPOLL_CTL_ADD, socket_fd, &event) == -1) {
                        error(1, errno, "epoll_ctl error");
                    }
                }
            } else {
                socket_fd = events[i].data.fd;
                char buffer[MAXLINE];
                while(1) {
                    int g = read(socket_fd, buffer, sizeof(buffer));
                    if (g < 0) {
                        if (errno != EAGAIN) {
                            fprintf(stderr, "read error");
                            close(events[i].data.fd);
                        }
                        break;
                    } else if (g == 0){
                        close(socket_fd);
                        break;
                    } else {
                        for (int i = 0; i < n; i++) {
                            buffer[i] = rot13_char(buffer[i]);
                        }
                        if (write(socket_fd, buffer, strlen(buffer)) < 0) {
                            error(1, errno, "write error");
                        }
                    }
                }
            }
        }
    }
}