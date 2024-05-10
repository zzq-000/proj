




include_directories(${CMAKE_SOURCE_DIR}/src/FEC)
include_directories(${CMAKE_SOURCE_DIR}/src/transport)
include_directories(${CMAKE_SOURCE_DIR}/src/util)
include_directories(${CMAKE_SOURCE_DIR}/third_party/glog/src)


add_executable(test test.cc)
target_link_libraries(test glog::glog gtest gtest_main fec pthread)

add_executable(client client.cc)
target_link_libraries(client util cm256 glog fec)

# add_executable(server server.cc)
# target_link_libraries(server util cm256 glog fec)


add_executable(test_packet test_packet.cc)
target_link_libraries(test_packet glog)

add_executable(test_fc test_fc.cc)
target_link_libraries(test_fc glog util)



add_executable(epoll epoll.cc)
add_executable(epoll_client epoll_client.cc)
target_link_libraries(epoll_client util glog::glog fec)

add_executable(server2.0 server2.0.cc)
target_link_libraries(server2.0 util glog::glog fec)


add_executable(test_blockFecCodec test_blockFecCodec.cc)
target_link_libraries(test_blockFecCodec gtest_main fec glog)


add_executable(test_cornercase test_cornercase.cc)
target_link_libraries(test_cornercase gtest_main fec glog)