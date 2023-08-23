add_executable(tcp-client tcpclient.c)
target_link_libraries(tcp-client yolanda)

add_executable(tcp-server tcpserver.c)
target_link_libraries(tcp-server yolanda)
