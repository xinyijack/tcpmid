#include "../lib/common.h"
//
// Created by jack on 23-8-23.
//
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

    int res1 = listen(listen_fd, LISTENQ);
    if (res1 < 0) {
        error(1, errno, "listen error");
    }

    signal(SIGINT, sig_int);
    signal(SIGPIPE, SIG_DFL);

    struct sockaddr_in client_addr;
    size_t client_len = sizeof(client_addr);
    int socked_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
    if (socked_fd < 0) {
        error(1, errno, "accept failed");
    }

    char message[MAXLINE];
    count = 0;

    while(1) {
        int n = read(socked_fd, message, MAXLINE);
        if (n < 0) {
            error(1, errno, "read failed");
        } else if (n == 0) {
            error(1, errno, "client closed");
        }
        message[n] = 0;
        printf("recv %d bytes: %s \n", n, message);

        char send_line[MAXLINE];
        sleep(5);

        sprintf(send_line, "Hi, %s", message);
        int wn = send(socked_fd, send_line, strlen(send_line), 0);
        printf("now send %d bytes", wn);
        if (wn < 0) {
            error(1, errno, "send failed");
        }
    }
}
