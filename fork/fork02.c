//
// Created by jack on 23-8-29.
//
#include "lib/common.h"

#define MAX_LINE 4096

char rot13_char(char c) {
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
        return c - 13;
    else
        return c;
}

void child_run(int fd) {
    char outbuf[MAX_LINE + 1];
    size_t outbuf_used = 0;
    ssize_t result;

    while (1) {
        char ch;
        result = recv(fd, &ch, 1, 0);
        if (result == 0) {
            break;
        } else if (result == -1) {
            perror("read");
            break;
        }

        /* We do this test to keep the user from overflowing the buffer. */
        if (outbuf_used < sizeof(outbuf)) {
            outbuf[outbuf_used++] = rot13_char(ch);
        }

        if (ch == '\n') {
            send(fd, outbuf, outbuf_used, 0);
            outbuf_used = 0;
            continue;
        }
    }
}


void sigchld_handler(int sig) {
    while (waitpid(-1, 0, WNOHANG) > 0);
    return;
}

int main(int c, char **v) {
    //为什么变量int socket_fd放在此处，会报socket program error：Socket operation on non-socket错误，放到while循环里面定义该变量却不会
    //int socket_fd
    int listen_fd = tcp_server_listen(SERV_PORT);
    signal(SIGCHLD, sigchld_handler);
    while(1) {
        struct sockaddr_storage client_addr;
        socklen_t client_len = sizeof(client_addr);
        int socket_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_len);
        if ((socket_fd < 0)) {
            error(1, errno, "accept error");
            exit(1);
        }
            //child thread
        if (fork() == 0) {
            close(listen_fd);
            child_run(socket_fd);
            exit(0);
        } else {
            //main thread
            close(socket_fd);
        }
    }
}