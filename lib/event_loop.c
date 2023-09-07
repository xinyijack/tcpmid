//
// Created by cryin on 2023/9/7.
//
#include "common.h"
#include "event_loop.h"
#include "channel_map.h"
#include <assert.h>

struct event_loop *event_loop_init_with_name(char *threadName) {
    struct event_loop* eventLoop = malloc(sizeof(struct event_loop));
    pthread_mutex_init(&eventLoop->mutex, NULL);
    pthread_cond_init(&eventLoop->cond, NULL);

    if (threadName == NULL) {
        eventLoop->thread_name = "Main Thread";
    } else {
        eventLoop->thread_name = threadName;
    }

    eventLoop->quit = 0;
    eventLoop->channelMap = malloc(sizeof(struct channel_map));
    map_init(eventLoop->channelMap);

#ifdef EPOLL_ENABLE
    yolanda_msgx("set epoll as dispatcher, %s", eventLoop->thread_name);
    eventLoop->dispatcher = &epoll_dispatcher;
#else
    yolanda_msgx("set poll as dispatcher, %s", eventLoop->thread_name);
    eventLoop->dispatcher = &poll_dispatcher;
#endif

    eventLoop->dispatcherData = eventLoop->dispatcher->init(eventLoop);
    eventLoop->ower_thread_id = pthread_self();

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, eventLoop->socketPair) < 0) {
        LOG_ERR("socketpair setup failed");
    }

    //链表初始化
    eventLoop->head = NULL;
    eventLoop->tail = NULL;
    eventLoop->is_handle_pending = 0;

    //将通知用的socket_pair注册进channelmap以及链表中
    struct channel *channel = channel_new(eventLoop->socketPair[1], EVENT_READ, handleWakeup, NULL, eventLoop);
    event_loop_add_channel_event(eventLoop, eventLoop->socketPair[1], channel);
    return eventLoop;
}

struct event_loop *event_loop_init() {
    return event_loop_init_with_name(NULL);
}

//处理链表中待处理的channel事件，is_handle_pending标志位置1，表示正在操作链表
int event_loop_handle_queue_channel(struct event_loop *eventLoop) {
    pthread_mutex_lock(&eventLoop->mutex);
    eventLoop->is_handle_pending = 1;

    //存入channel_map中
    struct channel_element *channelElement = eventLoop->head;
    while(channelElement != NULL) {
        if (channelElement->type == 1) {
            event_loop_handle_channel_map_add(eventLoop, channelElement->channel->fd, channelElement->channel);
        } else if (channelElement -> type == 2) {
            event_loop_handle_queue_remove(eventLoop, channelElement->channel->fd, channelElement->channel);
        } else if (channelElement->type == 3) {
            event_loop_handle_queue_update(eventLoop, channelElement->channel->fd, channelElement->channel);
        }
        channelElement = channelElement->next;
    }
    eventLoop->head = eventLoop->tail = NULL;
    eventLoop->is_handle_pending = 0;

    pthread_mutex_unlock(&eventLoop->mutex);
    return 0;
}

void event_loop_wakeup(struct event_loop *eventLoop) {
    char one = 'a';
    size_t ns = write(eventLoop->socketPair[0], &one, sizeof one);
    if (ns != sizeof one) {
        LOG_ERR("handle wake up error");
    }
    yolanda_msgx("wakeup, %s", eventLoop->thread_name);
}

//添加channel element到链表中
void event_loop_channel_buffer_nolock(struct event_loop *eventLoop, int fd, struct channel *channel0, int type) {
    struct channel_element *channelElement = malloc(sizeof(struct channel_element));
    channelElement->channel = channel0;
    channelElement->type = type;
    channelElement->next = NULL;

    if (eventLoop->head == NULL) {
        eventLoop->head = eventLoop->tail = channelElement;
    } else {
        eventLoop->tail = channelElement;
    }
}

int event_loop_do_channel_event(struct event_loop *eventLoop, int fd, struct channel *channel0, int type) {
    pthread_mutex_lock(&eventLoop->mutex);
    assert(eventLoop->is_handle_pending == 0);

    event_loop_channel_buffer_nolock(eventLoop, fd, channel0, type);
    pthread_mutex_unlock(&eventLoop->mutex);

    if (eventLoop->ower_thread_id != pthread_self()) {
        event_loop_wakeup(eventLoop);
    } else {
        event_loop_handle_queue_channel(eventLoop);
    }
    return 0;
}

int event_loop_add_channel_event(struct event_loop *eventLoop, int fd, struct channel *addChannel) {
    return event_loop_do_channel_event(eventLoop, fd, addChannel, 1);
}

int event_loop_delete_channel_event(struct event_loop *eventLoop, int fd, struct channel *addChannel) {
    return event_loop_do_channel_event(eventLoop, fd, addChannel, 2);
}

int event_loop_update_channel_event(struct event_loop *eventLoop, int fd, struct channel *addChannel) {
    return event_loop_do_channel_event(eventLoop, fd, addChannel, 3);
}

int event_loop_handle_channel_map_add(struct event_loop *eventLoop, int fd, struct channel *addChannel) {
    yolanda_msgx("add channel fd: %d, %s", fd, eventLoop->thread_name);
    struct channel_map *channelMap = eventLoop->channelMap;

    if (fd < 0) {
        return 0;
    }

    if (fd >= channelMap->nentries) {
        if (map_make_space(channelMap, fd, sizeof(struct channel *)) == -1) {
            return -1;
        }
    }

    if (channelMap->entries[fd] == NULL) {
        channelMap->entries[fd] = addChannel;
        struct event_dispatcher *dispatcher = eventLoop->dispatcher;
        dispatcher->add(eventLoop, addChannel);
        return 1;
    }
    return 0;
}

int event_loop_handle_channel_map_update(struct event_loop *eventLoop, int fd, struct channel *updateChannel) {
    yolanda_msgx("add channel fd: %d, %s", fd, eventLoop->thread_name);
    struct channel_map *channelMap = eventLoop->channelMap;

    if (fd < 0) {
        return 0;
    }

    if (channelMap->nentries == NULL) {
            return -1;
    }

    struct event_dispatcher *dispatcher = eventLoop->dispatcher;
    dispatcher->update(eventLoop, updateChannel);
}

int event_loop_handle_channel_map_delete(struct event_loop *eventLoop, int fd, struct channel *addChannel) {
    yolanda_msgx("add channel fd: %d, %s", fd, eventLoop->thread_name);
    assert(fd == addChannel->fd);
    struct channel_map *map = eventLoop->channelMap;

    if (fd < 0) {
        return 0;
    }

    if (fd >= map->nentries) {
        return -1;
    }

    struct channel *channel1 = map->entries[fd];
    int res = 0;
    if ((eventLoop->dispatcher->del(eventLoop, channel1)) == -1 ) {
        res = -1;
    } else {
        res = 1;
    }
    map->entries[fd] = NULL;
    return res;
}

//运行event_loop中基于poll或者epoll的无限循环事件分发流程
int event_loop_run(struct event_loop *eventLoop) {
    assert(eventLoop != NULL);

    struct event_dispatcher *dispatcher = eventLoop->dispatcher;

    if (eventLoop->ower_thread_id != pthread_self()) {
        exit(1);
    }

    yolanda_msgx("event_loop run %s", eventLoop->thread_name);
    struct timeval timeval;
    timeval.tv_sec = 1;

    while(!eventLoop->quit) {
        dispatcher->dispatch(eventLoop, timeval);
        event_loop_handle_queue_channel(eventLoop);
    }

    yolanda_msgx("event loop end %s", eventLoop->thread_name);
    return 0;
}

