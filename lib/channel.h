//
// Created by cryin on 2023/9/7.
//

#ifndef YOLANDA_CHANNEL_H
#define YOLANDA_CHANNEL_H

#define EVENT_TIMEOUT 0x01
#define EVENT_READ 0x02
#define EVENT_WRITE 0x04
#define EVENT_SIGNAL 0x08

#include "common.h"
#include "event_loop.h"
#include "buffer.h"
typedef int (*event_read_callback)(void *data);
typedef int (*event_write_callback)(void *data);

struct channel {
    int fd;
    int events;

    event_read_callback eventReadCallback;
    event_write_callback eventWriteCallback;
    void *data;
};

struct channel *channelNew(int fd, int events, event_read_callback eventReadCallback, event_write_callback eventWriteCallback, void *data);

int channel_write_event_is_enabled(struct channel *channel);

int channel_write_event_enable(struct channel *channel);

int channel_write_event_disable(struct channel *channel);
#endif //YOLANDA_CHANNEL_H
