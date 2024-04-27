include_directories(test PUBLIC ${PROJECT_SOURCE_DIR}/include)


add_executable(test test.cc)

target_link_libraries(test PUBLIC cm256)
# message(${PROJECT_SOURCE_DIR})



add_executable(client client.cc)
add_executable(server server.cc)

add_executable(fec FecEncoder.cc)
target_link_libraries(fec cm256 glog)
