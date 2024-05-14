cmake_minimum_required(VERSION 3.5)

project(UTS) # udp transport system

# set(CMKAE_C++_COMPILER /usr/bin/g++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

# message(${CMAKE_BUILD_RPATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)

add_subdirectory(third_party)
add_subdirectory(src)
# add_subdirectory(test)


# find_package(protobuf)
# message(${PROTOBUF})

# add cm256
# include_directories(${PROJECT_SOURCE_DIR}/third_party/cm256/include)
# link_directories(${PROJECT_SOURCE_DIR}/third_party/cm256/build)


# add googletest
# include_directories(${PROJECT_SOURCE_DIR}/third_party/googletest/googletest/include)
# include_directories(${PROJECT_SOURCE_DIR}/third_party/googletest/googlemock/include)
# link_directories(${PROJECT_SOURCE_DIR}/third_party/googletest/build/lib)

# add glog
# include_directories(${PROJECT_SOURCE_DIR}/third_party/glog/src)
# link_directories(${PROJECT_SOURCE_DIR}/third_party/glog/build)

# 设置全局的头文件搜索路径


# set_property(GLOBAL PROPERTY INCLUDE_DIRECTORIES
#     ${PROJECT_SOURCE_DIR}/third_party/cm256/include
#     ${PROJECT_SOURCE_DIR}/third_party/googletest/googletest/include
#     ${PROJECT_SOURCE_DIR}/third_party/googletest/googlemock/include
#     ${PROJECT_SOURCE_DIR}/third_party/glog/src
# )

# # 设置全局的库文件搜索路径
# set_property(GLOBAL PROPERTY LINK_DIRECTORIES
#     ${PROJECT_SOURCE_DIR}/third_party/cm256/build
#     ${PROJECT_SOURCE_DIR}/third_party/googletest/build/lib
#     ${PROJECT_SOURCE_DIR}/third_party/glog/build
# )

# # cm256 库路径
# set(CM256_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/third_party/cm256/include)
# set(CM256_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/third_party/cm256/build)

# # googletest 库路径
# set(GOOGLETEST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/third_party/googletest/googletest/include)
# set(GOOGLETEST_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/third_party/googletest/build/lib)

# # glog 库路径
# set(GLOG_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/third_party/glog/src)
# set(GLOG_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/third_party/glog/build)
