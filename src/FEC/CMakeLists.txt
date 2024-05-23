
# include_directories(${PROJECT_SOURCE_DIR}/third_party/cm256/include)
# link_directories(${PROJECT_SOURCE_DIR}/third_party/cm256/build)

find_package(glog REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(GTest REQUIRED)
# find_package(absl REQUIRED)


# message(${CMAKE_CURRENT_BINARY_DIR})
# set(PROTO_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/")

add_library(fec_type OBJECT "${CMAKE_CURRENT_LIST_DIR}/fec_type.proto")
target_include_directories(fec_type INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(fec_type PUBLIC protobuf::libprotobuf)


# message(${CMAKE_CURRENT_LIST_DIR})
protobuf_generate(
    TARGET fec_type
    IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}"
    PROTOC_OUT_DIR "${CMAKE_CURRENT_LIST_DIR}"
)

add_library(fec block_fec_codec.cc fec_util.cc)
target_include_directories(fec INTERFACE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(fec PUBLIC glog::glog cm256 fec_type)

# add_executable(test_codec test.cc)
# target_link_libraries(test_codec GTest::gtest_main fec)
# absl::log_internal_check_op

