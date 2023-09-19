add_executable(poll-client pollclient.c)
target_link_libraries(poll-client xinyijack)

add_executable(poll-server pollserver.c)
target_link_libraries(poll-server xinyijack)
