#include "../lib/common.h"
#define MAXCLINT 128
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
    socklen_t client_len = sizeof(client_addr);

    char message[MAXLINE];
    count = 0;
    fd_set allreads, readmask;
    FD_ZERO(&allreads);
    FD_SET(listen_fd, &allreads);
    int ready_fds;
    int max_fd = listen_fd;
    int fds[MAXCLINT];
    int i;
    while(1) {
        readmask = allreads;
        for (int k = 0; k < MAXCLINT; k++) {
            if (fds[k] > 0) {
                FD_SET(fds[k], &readmask);
            }
        }

        int socked_fd;
        if ((ready_fds = select(max_fd + 1, &readmask, NULL, NULL, NULL)) < 0) {
            error(1, errno, "select error");
        }
        if (FD_ISSET(listen_fd, &readmask)) {
            socked_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
            if (socked_fd < 0) {
                error(1, errno, "accept failed");
            }
            for (i = 0; i < MAXCLINT; i++) {
                if (fds[i] == 0) {
                    fds[i] = socked_fd;
                    break;
                }
            }
            if (i == MAXCLINT) {
                printf("can not hold too many clients");
                write(socked_fd, "can not hold too many clients", strlen("can not hold too many clients"));
                close(socked_fd);
            }
            if (socked_fd > max_fd) {
                max_fd = socked_fd;
            }
            if (--ready_fds <= 0) {
                continue;
            }
        }

        for (int k = 0; k < MAXCLINT; k++) {
            if (FD_ISSET(fds[k], &readmask)) {
                int n = read(socked_fd, message, MAXLINE);
                if (n < 0) {
                    error(1, errno, "read failed");
                } else if (n == 0) {
                    error(1, errno, "client closed");
                }
                message[n] = 0;
                printf("recv %d bytes: %s \n", n, message);
                //执行收到的命令，并在终端执行，获取执行的输出
                char send_line[MAXLINE];
                sleep(1);
                FILE * f;
                f = popen(message, "r");

                while (fgets(send_line, sizeof(send_line), f) != NULL) {
                    int wn = send(socked_fd, send_line, strlen(send_line), 0);
                    printf("now send %d bytes", wn);
                    if (wn < 0) {
                        error(1, errno, "send failed");
                    }
                }
                pclose(f);
                if (--ready_fds <= 0) {
                    break;
                }
            }
        }


    }
}
