
# include_directories(${PROJECT_SOURCE_DIR}/src/util)
# include_directories(${PROJECT_SOURCE_DIR}/src/FEC)
# include_directories(${PROJECT_SOURCE_DIR}/third_party/glog/src)

find_package(Protobuf CONFIG REQUIRED)


add_library(packet OBJECT "${CMAKE_CURRENT_LIST_DIR}/packet.proto")
target_include_directories(packet PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(packet INTERFACE ${CMAKE_CURRENT_LIST_DIR} ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(packet PUBLIC protobuf::libprotobuf)


message(${CMAKE_CURRENT_BINARY_DIR})
# 生成 protobuf 文件
protobuf_generate(
    TARGET packet
    IMPORT_DIRS "${PROJECT_SOURCE_DIR}/src/transport" "${PROJECT_SOURCE_DIR}/src/"
    PROTOC_OUT_DIR "${CMAKE_CURRENT_LIST_DIR}"
)


# add_executable(test2 test.cc)
# target_link_libraries(test2 packet fec_type)

# add_library(cach packet_cache.cc)