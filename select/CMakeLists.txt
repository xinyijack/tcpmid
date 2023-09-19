add_executable(tcp-client selectclient.c)
target_link_libraries(tcp-client xinyijack)

add_executable(tcp-server selectserver.c)
target_link_libraries(tcp-server xinyijack)
