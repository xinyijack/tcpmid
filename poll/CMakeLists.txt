add_executable(tcp-client pollclient.c)
target_link_libraries(tcp-client yolanda)

add_executable(tcp-server pollserver.c)
target_link_libraries(tcp-server yolanda)
