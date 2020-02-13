cmake_minimum_required(VERSION 3.4.1)

project(IOCanary)

#aux_source_directory(../matrix-android-commons/src/main/cpp/elf_hook ELF_HOOK)
#include_directories(../matrix-android-commons/src/main/cpp/elf_hook)

# 去 libxhook 的目录查找 CMakeLists.txt 文件，并编译输出到指定目录
add_subdirectory(./src/main/cpp/libxhook ${CMAKE_CURRENT_BINARY_DIR}/libxhook)

aux_source_directory(src/main/cpp/detector/ SRC_DIR_DETECTOR)
aux_source_directory(src/main/cpp/comm/ SRC_DIR_COMM)
aux_source_directory(src/main/cpp/core/ SRC_DIR_CORE)
aux_source_directory(src/main/cpp/md5/ MD5_SRCS)
include_directories(src/main/cpp/md5)
include_directories(src/main/cpp)

add_library(io-canary
        SHARED
        ${SRC_DIR_COMM}
        ${SRC_DIR_DETECTOR}
        ${SRC_DIR_CORE}
        ${MD5_SRCS}
        src/main/cpp/io_canary_jni.cc
        )

find_library(log-lib
        log)

target_link_libraries(io-canary
        ${log-lib}
        xhook)