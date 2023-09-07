add_executable(epoll-client epollclient.c)
target_link_libraries(epoll-client yolanda)

add_executable(epoll-server epollserver.c)
target_link_libraries(epoll-server yolanda)
