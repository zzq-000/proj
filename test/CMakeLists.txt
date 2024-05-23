

include_directories(${PROJECT_SOURCE_DIR}/third_party/cm256/include)
link_directories(${PROJECT_SOURCE_DIR}/third_party/cm256/build)


find_package(GTest REQUIRED)
find_package(glog REQUIRED)


add_executable(unittest 
    test_fec_functionality.cc
    test_packet_functionality.cc
    test_packet_cache_functionality.cc

)
target_link_libraries(unittest GTest::gtest_main fec glog::glog transport)
