cmake_minimum_required(VERSION 3.21)
project(discrete)

set(CMAKE_CXX_STANDARD 23)
link_directories(lib)
add_compile_definitions(DISCRETE_DEBUG)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(COCOA_LIB Cocoa REQUIRED)
    find_library(OPEN_GL_LIB OpenGl REQUIRED)
    find_library(IOKIT_LIB IOKit REQUIRED)
    find_library(CORE_VIDEO_LIB CoreVideo REQUIRED)
    link_libraries(libboost_unit_test_framework.dylib ${OPEN_GL_LIB} ${COCOA_LIB} ${IOKIT_LIB} ${CORE_VIDEO_LIB})
endif()

link_libraries(libglad.a libglfw3.a libjsoncpp.a libspdlog.a)

include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src/main)

file(GLOB_RECURSE SOURCES "src/main/*")

add_executable(mroneillscubes ${SOURCES} src/main/server/server.h)



