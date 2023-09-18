//
// Created by cryin on 2023/9/18.
//
#include "channel.h"

struct channel *new_channel(int fd, int events, event_write_callBack writeCallBack, event_read_callBack readCallBack, void *data) {
    struct channel *chan = malloc(sizeof(struct channel));
    chan->fd = fd;
    chan->events = events;
    chan->write = writeCallBack;
    chan->read = readCallBack;
    chan->data = data;
    return chan;
}
