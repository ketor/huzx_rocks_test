cmake_minimum_required(VERSION 2.8.10)
project(huzx_rocks_test C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(THIRD_PARTY_PATH ${CMAKE_CURRENT_BINARY_DIR}/third-party)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_PREFIX_PATH ${OUTPUT_PATH})

include(zlib)
include(gflags)
include(snappy)
include(rocksdb)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# include PROTO_HEADER
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${ROCKSDB_INCLUDE_DIR})

set(DYNAMIC_LIB
    ${GFLAGS_LIBRARIES}
    ${ROCKSDB_LIBRARIES}
    ${SNAPPY_LIBRARIES}
    ${ZLIB_LIBRARIES}
    )

set(DEPEND_LIBS
    gflags
    rocksdb
    snappy
    zlib
    )

add_executable(huzx_rocksdb huzx_rocksdb.cc)
add_dependencies(huzx_rocksdb ${DEPEND_LIBS})

target_link_libraries(huzx_rocksdb
                      "-Xlinker \"-(\""
                      ${DYNAMIC_LIB}
                      "-Xlinker \"-)\"")
