cmake_minimum_required(VERSION 3.21)
project(mroneillscubes)

set(CMAKE_CXX_STANDARD 23)
link_directories(lib)

link_libraries( libglad.a libglfw3.a libglm_static.a libjsoncpp.a)
include_directories(${CMAKE_SOURCE_DIR}/include)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(COCOA_LIB Cocoa REQUIRED)
    find_library(OPEN_GL_LIB OpenGl REQUIRED)
    find_library(IOKIT_LIB IOKit REQUIRED)
    find_library(CORE_VIDEO_LIB CoreVideo REQUIRED)
    link_libraries(${OPEN_GL_LIB} ${COCOA_LIB} ${IOKIT_LIB} ${CORE_VIDEO_LIB})
endif()


add_executable(mroneillscubes client/Main.cpp server/Server.cpp server/Server.h server/business/World.h server/business/World.cpp server/business/Voxel.cpp server/business/Voxel.h client/render/RenderConfiguration.h client/render/RenderClient.h client/render/RenderClient.cpp)



