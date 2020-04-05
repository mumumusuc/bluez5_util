cmake_minimum_required(VERSION 3.0)
project("bluez_util")

set(CMAKE_CXX_STANDARD 14)

find_package(PkgConfig)
#[[
pkg_check_modules(DBUS dbus-1) 
include_directories(${DBUS_INCLUDE_DIRS})
link_directories(${DBUS_LIBRARY_DIRS})
message(STATUS "dbus -> ${DBUS_LIBRARIES}")

pkg_check_modules(GBUS dbus-glib-1) 
include_directories(${GBUS_INCLUDE_DIRS})
link_directories(${GBUS_LIBRARY_DIRS})
message(STATUS "gbus -> ${GBUS_LIBRARIES}")

pkg_check_modules(GLIB glib-2.0) 
include_directories(${GLIB_INCLUDE_DIRS})
link_directories(${GLIB_LIBRARY_DIRS})
message(STATUS "glib -> ${GLIB_LIBRARIES}")
]]

pkg_check_modules(GIO gio-2.0) 
include_directories(${GIO_INCLUDE_DIRS})
link_directories(${GIO_LIBRARY_DIRS})
message(STATUS "gio -> ${GIO_LIBRARIES}")

pkg_check_modules(PY3 python3) 
include_directories(${PY3_INCLUDE_DIRS})
link_directories(${PY3_LIBRARY_DIRS})
message(STATUS "python3 -> ${PY3_INCLUDE_DIRS}")

add_executable(${PROJECT_NAME} main.cc util.cc)
target_link_libraries(${PROJECT_NAME} ${PY3_LIBRARIES})

add_executable(test test.cc gutil.cc)
target_link_libraries(test ${GIO_LIBRARIES})
