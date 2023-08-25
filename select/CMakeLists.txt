add_executable(tcp-client selectclient.c)
target_link_libraries(tcp-client yolanda)

add_executable(tcp-server selectserver.c)
target_link_libraries(tcp-server yolanda)
