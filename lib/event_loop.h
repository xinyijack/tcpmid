//
// Created by cryin on 2023/9/7.
//
#include <pthread.h>
#include "common.h"
#ifndef YOLANDA_EVENT_LOOP_H
#define YOLANDA_EVENT_LOOP_H

#endif //YOLANDA_EVENT_LOOP_H
extern const struct event_dispatcher poll_dispatcher;
extern const struct event_dispatcher epoll_dispatcher;

struct channel_element {
    int type;
    struct channel *channel;
    struct channel_element *next;
};

struct event_loop {
    //退出信号
    int quit;

    //事件分发器及分发器的数据，包括事件数组events和描述符fd
    const struct event_dispatcher *dispatcher;
    struct event_dispatcher_data *dispatcherData;

    //channel map使用fd找到channel
    struct channel_map *channelMap;

    //event_loop待处理的事件链表，记录头尾节点
    int is_handle_pending;
    struct channel_element *head;
    struct channel_element *tail;

    //线程相关属性
    pthread_t ower_thread_id;
    char *thread_name;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int socketPair[2];
};

struct event_loop *event_loop_init();

struct event_loop *event_loop_init_with_name(char *thread_name);

int event_loop_run(struct event_loop *eventLoop);

void event_loop_wakeup(struct event_loop *eventLoop);

//Channel的增删改查
int event_loop_add_channel_event(struct event_loop *eventLoop, int fd, struct channel *addChannel);
int event_loop_delete_channel_event(struct event_loop *eventLoop, int fd, struct channel *addChannel);
int event_loop_update_channel_event(struct event_loop *eventLoop, int fd, struct channel *addChannel);

//待处理的channel队列的处理方法
int event_loop_handle_channel_map_add(struct event_loop *eventLoop, int fd, struct channel *addChannel);
int event_loop_handle_channel_map_delete(struct event_loop *eventLoop, int fd, struct channel *addChannel);
int event_loop_handle_channel_map_update(struct event_loop *eventLoop, int fd, struct channel *updateChannel);

//执行待处理channel的相关事件所绑定的函数方法
int channelEventActive(struct event_loop *eventLoop, int fd, int res);