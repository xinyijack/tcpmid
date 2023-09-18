//
// Created by cryin on 2023/9/18.
//

#ifndef YOLANDA_EVENTLOOP_H
#define YOLANDA_EVENTLOOP_H
struct event_loop {
    int quit;

    struct dispatcher;
    void *dispatch_data;

    //ChannelElement链表
    struct ChannelNode *head;
    struct ChannelNode *tail;

    //查询Channel的Map
    struct Channel *channel_map;
};

//默认初始化函数
struct event_loop *init();
//带参数的初始化函数
struct event_loop *init_with_thread_name(char *thread_name);

//ChannelNode链表的增删改函数
int add_channel_node(struct event_loop *eventLoop, struct Channel *channel0);

int remove_channel_node(struct event_loop *eventLoop, struct Channel *channel0);

int update_channel_node(struct event_loop *eventLoop, struct Channel *channel0);

//ChannelMap的增删改函数
int add_channel_to_map(struct event_loop *eventLoop, int fd, struct Channel *channel1);

int remove_channel_from_map(struct event_loop *eventLoop, int fd, struct Channel *channel1);

int update_channel_in_map(struct event_loop *eventLoop, int fd, struct Channel *channel1);

#endif //YOLANDA_EVENTLOOP_H
