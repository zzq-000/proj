
# add glog
# include_directories(${PROJECT_SOURCE_DIR}/third_party/glog/src/glog)
#                                 # /app/proj/third_party/glog/src
# link_directories(${PROJECT_SOURCE_DIR}/third_party/glog/build)

# message(${PROJECT_SOURCE_DIR}/third_party/glog/build)
# add cm256
# include_directories(${PROJECT_SOURCE_DIR}/third_party/cm256/include)
# link_directories(${PROJECT_SOURCE_DIR}/third_party/cm256/build)

# include_directories(
#     ${CM256_INCLUDE_DIR}
#     ${GLOG_INCLUDE_DIR}
# )

# link_directories(
#     ${CM256_LIBRARY_DIR}
#     ${GLOG_LIBRARY_DIR}
# )

add_library(fec SHARED BlockFecCodec.cc)
target_link_libraries(fec glog cm256)