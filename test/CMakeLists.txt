

# include_directories(${PROJECT_SOURCE_DIR}/third_party/cm256/include)
# link_directories(${PROJECT_SOURCE_DIR}/third_party/cm256/build)


find_package(GTest REQUIRED)
find_package(glog REQUIRED)


add_executable(unittest 
    # test_fec_functionality.cc
    # test_packet_functionality.cc
    # test_packet_cache_functionality.cc
    # test_packet_fec_encode.cc
    test_sworker_functionality.cc
    # test_sworker_and_rworker.cc
    test_packet_logger.cc
    test_ge_channel.cc
)
target_link_libraries(unittest GTest::gtest_main glog::glog transport fec util)
