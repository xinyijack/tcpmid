#include <stdio.h>
#include "lib/common.h"

int main(int argc, char **argv) {
    //create socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    //config socket_addr, init socket_addr to zero
    struct sockaddr_in socket_addr;
    bzero(&socket_addr, sizeof(socket_addr));
    //convert ip addr to special form
    inet_pton(AF_INET, argv[1], &socket_addr.sin_addr);
    socket_addr.sin_port = htons(SERV_PORT);
    socket_addr.sin_family = AF_INET;

    int socket_len = sizeof(socket_addr);
    //connect
    int con_res = connect(socket_fd, (struct sockaddr *) &socket_addr, socket_len);
    if (con_res < 0) {
        error(1, errno, "connect failed");
    }
    char send_line[MAXLINE];
    char recv_line[MAXLINE + 1];
    int n;

    fd_set readall;
    //loop temp x
    fd_set readmask;
    FD_ZERO(&readall);
    FD_SET(0, &readall);
    FD_SET(socket_fd, &readall);
    //write and read
    while(1) {
        readmask = readall;
        int nr = select(socket_fd + 1, &readmask, NULL, NULL, NULL);
        if (nr < 0) {
            error(1, errno, "select error");
        } else if (nr == 0) {
            error(1, errno, "no ready fd");
        }
        if (FD_ISSET(socket_fd, &readmask)) {
            n = read(socket_fd, &recv_line, MAXLINE);
            if (n < 0) {
                error(1, errno, "read failed");
            } else if (n == 0) {
                error(1, errno, "server closed")
            }
            recv_line[n] = 0;
            fputs(recv_line, stdout);
            fputs("\n", stdout);
        }
        if (FD_ISSET(0, &readmask)) {
            if (fgets(send_line, MAXLINE, stdin) != NULL) {
                if (strncmp(send_line, "shutdown", 8) == 0) {
                    FD_CLR(0, &readall);
                    if (shutdown(socket_fd, 1)) {
                        error(1, errno, "shutdown failed");
                    }
                } else if (strncmp(send_line, "closed", 5) == 0) {
                    FD_CLR(0, &readall);
                    if (close(socket_fd)) {
                        error(1, errno, "closed failed");
                    }
                    sleep(6);
                    exit(0);
                } else {
                    int k = sizeof(send_line);
                    if (send_line[k - 1] == '\n') {
                        send_line[k - 1] = 0;
                    }

                    printf("now sending %s \n", send_line);
                    size_t tn = write(socket_fd, send_line, k);
                    if (tn < 0) {
                        error(1, errno, "write failed");
                    }
                    printf("send bytes %zu \n", tn);
                }
            }
        }
    }
}
