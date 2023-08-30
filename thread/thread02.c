//
// Created by jack on 23-8-29.
//
#include "lib/common.h"

extern void loop_echo(int);

void thread_run(int fd) {
    pthread_detach(pthread_self());
//    int fd = (int) arg;
    loop_echo(fd);
}


int main(int c, char **v) {
    //为什么变量int socket_fd放在此处，会报socket program error：Socket operation on non-socket错误，放到while循环里面定义该变量却不会
    //int socket_fd
    int listen_fd = tcp_server_listen(SERV_PORT);
    pthread_t tid;

    while(1) {
        struct sockaddr_storage ss;
        socklen_t slen = sizeof(ss);
        int fd = accept(listen_fd, (struct sockaddr *) &ss, &slen);
        if (fd < 0) {
            error(1, errno, "accept error");
        } else {
            //child thread
            pthread_create(&tid, NULL, &thread_run, fd);
        }
    }
    return 0;
}