

include_directories(${PROJECT_SOURCE_DIR}/third_party/cm256/include)
link_directories(${PROJECT_SOURCE_DIR}/third_party/cm256/build)


find_package(GTest REQUIRED)
find_package(glog REQUIRED)


add_executable(test_fec_functionality test_fec_functionality.cc)
target_link_libraries(test_fec_functionality GTest::gtest_main fec)
