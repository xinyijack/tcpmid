//
// Created by cryin on 2023/9/18.
//

#ifndef YOLANDA_ACCEPTOT_H
#define YOLANDA_ACCEPTOT_H
#include "common.h"
struct acceptor{
    int listen_socket;
    int listen_port;
};

struct acceptor *init(int port);
#endif //YOLANDA_ACCEPTOT_H
