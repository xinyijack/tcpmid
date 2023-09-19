#include "utils.h"
#include "log.h"
#include "assert.h"


void assertInSameThread(struct event_loop *eventLoop) {
    if (eventLoop->owner_thread_id != pthread_self()) {
        LOG_ERR("not in the same thread");
        exit(-1);
    }
}

//1： same thread: 0： not the same thread
int isInSameThread(struct event_loop *eventLoop){
    return eventLoop->owner_thread_id == pthread_self();
}

static const char* memmem(const char* haystack, size_t hlen, const char* needle, size_t nlen)
{
    const char* cur;
    const char* last;
    assert(haystack);
    assert(needle);
    assert(nlen > 1);
    last =  haystack + hlen - nlen;
    for (cur = haystack; cur <= last; ++cur) {
        if (cur[0] == needle[0] && memcmp(cur, needle, nlen) == 0) {
            return cur;
        }
    }
    return NULL;
}