#include "../lib/common.h"
//
// Created by jack on 23-8-23.
//
#define MAX_CLINT 5
#define MAXEVEVTS 128
static int count;

static void sig_int(int sign) {
    printf("\nreceived %d datagrams\n", count);
    exit(0);
}

char rot13_char(char c) {
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
        return c - 13;
    else
        return c;
}

int main() {
//    if (argc != 2) {
//        error(1, errno, "param count error");
//    }
    int listen_fd, socket_fd;
    //create socket
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    //create socket addr
    struct sockaddr_in socket_addr;
    bzero(&socket_addr, sizeof(socket_addr));
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_port = htons(SERV_PORT);
    //meijia htonl
    socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int res0 = bind(listen_fd, (struct sockaddr *)&socket_addr, sizeof(socket_addr));
    if (res0 < 0) {
        error(1, errno, "bind error");
    }

    int res1 = listen(listen_fd, MAX_CLINT);
    if (res1 < 0) {
        error(1, errno, "listen error");
    }

    signal(SIGINT, sig_int);
    signal(SIGPIPE, SIG_DFL);


    struct epoll_event event;
    struct epoll_event *events;
    int efd;
    if ((efd = epoll_create1(0)) == -1) {
        error(1, errno, "epoll error");
    }
    event.data.fd = listen_fd;
    event.events = EPOLLET | EPOLLIN;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, listen_fd, &event) == -1) {
        error(1, errno, "epoll add listen fd failed");
    }
    char send_line[MAXLINE];
    int ready_fd;
    events = calloc(MAXEVEVTS, sizeof(event));
    while(1) {
        ready_fd = epoll_wait(efd, events, MAXEVEVTS, -1);
        printf("epoll wake up");
        for (int i = 0; i < ready_fd; i++) {
            if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP || !(events[i].events & EPOLLIN)) {
                fprintf(stderr, "epoll error");
                close(events[i].data.fd);
                continue;
            } else if (events[i].data.fd == listen_fd) {
                int conn_fd;
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                if ((conn_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_len)) < 0) {
                    error(1, errno, "accept error");
                }
                make_nonblocking(conn_fd);
                event.data.fd = conn_fd;
                event.events = EPOLLIN;
                if (epoll_ctl(efd, EPOLL_CTL_ADD, conn_fd, &event) == -1) {
                    error(1, errno, "epoll ctl failed");
                }
                continue;
            } else {
                socket_fd = events[i].data.fd;
                while(1) {
                    int n = read(socket_fd, send_line, sizeof(send_line));
                    if (n < 0) {
                        if (errno == EAGAIN) {
                            error(1, errno, "read error");
                            close(socket_fd);
                            break;
                        }
                    } else if (n == 0) {
                        close(socket_fd);
                        break;
                    } else {
                        for (i = 0; i < n; i++) {
                            send_line[i] = rot13_char(send_line[i]);
                        }
                        if (write(socket_fd, send_line, strlen(send_line)) < 0) {
                            error(1, errno, "write error");
                        }
                    }
                }
            }
        }
    }
    free(events);
    close(listen_fd);
}
