//
// Created by cryin on 2023/9/18.
//

#ifndef YOLANDA_CHANNEL_H
#define YOLANDA_CHANNEL_H

#include "common.h"
typedef int (*event_read_callBack)(void *data);
typedef int (*event_write_callBack)(void *data);
//专门处理Channel的读写事件
struct channel{
    int fd;
    int events;

    event_read_callBack read;
    event_write_callBack write;
    void *data;
};
#endif //YOLANDA_CHANNEL_H

//channel new函数
struct channel *new_channel(int fd, int events, event_write_callBack writeCallBack, event_read_callBack readCallBack, void *data);