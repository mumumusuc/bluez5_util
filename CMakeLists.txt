cmake_minimum_required(VERSION 3.0)
project("bluez_util")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS                "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG          "-g")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

find_package(PkgConfig)

pkg_check_modules(GIO gio-2.0) 
include_directories(${GIO_INCLUDE_DIRS})
link_directories(${GIO_LIBRARY_DIRS})
message(STATUS "gio -> ${GIO_LIBRARIES}")

add_executable(test test.cc gutil.cc)
target_link_libraries(test ${GIO_LIBRARIES} "pthread")
