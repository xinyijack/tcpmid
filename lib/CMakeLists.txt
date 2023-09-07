if (EPOLL_EXISTS)
    set(yolanda_lib
            log.c
            sock_ntop.c
            read.c
            tcp_server.c
            tcp_client.c
            thread_pool.c
            utils.c
            buffer.c
            tcp_connection.c
            http_server.c
            http_request.c
            http_response.c
            event_loop.h
            event_loop.c
    )
else ()
    set(yolanda_lib
            log.c
            sock_ntop.c
            read.c
            tcp_server.c
            tcp_client.c
            thread_pool.c
            utils.c
            buffer.c
            tcp_connection.c
            http_server.c
            http_request.c
            http_response.c
            ../cmd/run.c
            ../epoll/epollserver.c
            ../epoll/epollserver.c
            ../fork/fork01.c
            ../fork/fork01.c
            ../thread/thread01.c
            ../httpserver/http_server02.c
            event_loop.h
            event_loop.c
            channel_map.h
            channel_map.c
            event_dispatcher.h
            channel.h
            channel.c
            event_dispatcher.c
    )
endif ()


add_library(yolanda ${yolanda_lib})
target_link_libraries(yolanda pthread)

install(TARGETS yolanda DESTINATION lib)

#file(GLOB HEADERS "*.h")
#install(FILES ${HEADERS} DESTINATION include/)

