set(SRC_LIST calluserservice.cc ../user.pb.cc)

add_executable(consumer ${SRC_LIST})

target_link_libraries(consumer mprpc protobuf)

set(SRC_LIST2 callfriendservice.cc ../friend.pb.cc)

add_executable(consumer2 ${SRC_LIST2})

target_link_libraries(consumer2 mprpc protobuf)