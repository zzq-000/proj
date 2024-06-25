
# include_directories(${PROJECT_SOURCE_DIR}/third_party/cm256/include)
# link_directories(${PROJECT_SOURCE_DIR}/third_party/cm256/build)

find_package(glog REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(GTest REQUIRED)
# find_package(absl REQUIRED)


# message(${CMAKE_CURRENT_BINARY_DIR})
# set(PROTO_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/")
set(PROTO_LIB "cipher_type")
set(CIPHER_LIB "cipher")
add_library(${PROTO_LIB} OBJECT "${CMAKE_CURRENT_LIST_DIR}/cipher_type.proto")
target_include_directories(${PROTO_LIB} INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${PROTO_LIB} PUBLIC protobuf::libprotobuf)


# message(${CMAKE_CURRENT_LIST_DIR})
protobuf_generate(
    TARGET ${PROTO_LIB}
    IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}"
    PROTOC_OUT_DIR "${CMAKE_CURRENT_LIST_DIR}"
)

add_library(${CIPHER_LIB} cipher_impl.cc cipher.cc)
target_include_directories(${CIPHER_LIB} INTERFACE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(${CIPHER_LIB} PUBLIC ${PROTO_LIB} ssl crypto)

# add_executable(test_codec test.cc)
# target_link_libraries(test_codec GTest::gtest_main fec)
# absl::log_internal_check_op

