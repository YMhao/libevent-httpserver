cmake_minimum_required(VERSION 2.6)
set(APP httpServer)
project(${APP})

set(CMAKE_CXX_FLAG "-g -Wall")

AUX_SOURCE_DIRECTORY(./ SOURCES)

add_executable(${APP} ${SOURCES})

TARGET_LINK_LIBRARIES(${APP} pthread event event_pthreads)
