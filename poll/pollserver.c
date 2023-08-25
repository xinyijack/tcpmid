#include "../lib/common.h"
//
// Created by jack on 23-8-23.
//
#define MAX_CLINT 5
static int count;

static void sig_int(int sign) {
    printf("\nreceived %d datagrams\n", count);
    exit(0);
}

int main(int argc, char **argv) {
//    if (argc != 2) {
//        error(1, errno, "param count error");
//    }

    //create socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

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

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    char message[MAXLINE];
    count = 0;
    fd_set allreads, readmask;
    FD_ZERO(&allreads);
    FD_SET(listen_fd, &allreads);
    //
    struct pollfd fd[MAX_CLINT];
    fd[0].fd = listen_fd;
    fd[0].events = POLLRDNORM;

    char send_line[MAXLINE];
    int i;
    for (i = 1; i < MAX_CLINT; i++) {
        fd[i].fd = -1;
    }
    int ready_fd;
    while(1) {
        if ((ready_fd = poll(fd, MAX_CLINT, -1)) < 0) {
            error(1, errno, "poll error");
        }
        if (fd[0].revents & (POLLRDNORM | POLLERR)) {
            int conn_fd;
            conn_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_len)) < 0);
            for (i = 1; i < MAX_CLINT; i++) {
                if (fd[i].fd < 0) {
                    fd[i].fd = conn_fd;
                    fd[i].events = POLLRDNORM;
                    break;
                }
            }
            if (i == MAX_CLINT) {
                close(conn_fd);
                error(1, errno, "cannot hold so many clients");
            }
            if (--ready_fd <= 0) {
                continue;
            }
        }

        for (i = 1; i < MAX_CLINT; i++) {
            int socket_fd = fd[i].fd;
            if (socket_fd < 0) {
                continue;
            }
            if (fd[i].revents & (POLLRDNORM | POLLERR)) {
                int n = read(socket_fd, send_line, MAXLINE);
                if (n < 0) {
                    error(1, errno, "read error");
                } else if (n == 0) {
                    close(socket_fd);
                    fd[i].fd = -1;
                }
                if (--ready_fd < 0) {
                    break;
                }
            }
        }
    }
}
