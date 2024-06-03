find_package(glog REQUIRED)

add_executable(generate_trace generate_trace.cc)
target_link_libraries(generate_trace util glog::glog)

