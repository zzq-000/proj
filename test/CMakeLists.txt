

add_executable(test_ test.cc)
target_include_directories(test_ PUBLIC ${CMAKE_SOURCE_DIR}/src/FEC)
target_link_libraries(test_ gtest gtest_main fec pthread)