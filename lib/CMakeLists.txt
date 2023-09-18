if (EPOLL_EXISTS)
    set(yolanda_lib
            log.c
            sock_ntop.c
            read.c
            tcp_server.c
            tcp_client.c
            utils.c
            buffer.c
            tcp_connection.c
            channel.c
            acceptot.h
            acceptor.c
    )
else ()
    set(yolanda_lib
            log.c
            sock_ntop.c
            read.c
            tcp_server.c
            tcp_client.c
            utils.c
            buffer.c
            tcp_connection.c
            ../cmd/run.c
            ../epoll/epollserver.c
            ../epoll/epollserver.c
            ../fork/fork01.c
            ../fork/fork01.c
            ../thread/thread01.c
            ../httpserver/http_server02.c
            event_loop.h
            channel.h
            channel.c
            acceptot.h
            acceptor.c
    )
endif ()


add_library(yolanda ${yolanda_lib})
target_link_libraries(yolanda pthread)

install(TARGETS yolanda DESTINATION lib)

#file(GLOB HEADERS "*.h")
#install(FILES ${HEADERS} DESTINATION include/)

