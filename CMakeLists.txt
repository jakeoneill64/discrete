cmake_minimum_required(VERSION 3.21)
project(mroneillscubes)

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


add_executable(mroneillscubes src/main/client/Main.cpp src/main/client/render/render.h src/main/client/render/render.cpp src/main/client/render/Camera.h src/main/client/render/Camera.cpp src/main/client/render/Shader.h src/main/client/render/Shader.cpp src/main/Log.h src/main/Log.cpp src/main/client/Client.h src/main/client/Client.cpp src/main/client/input/InputConfiguration.h src/main/client/input/InputConfiguration.cpp src/main/client/input/InputManager.h src/main/client/input/InputManager.cpp src/main/client/Voxel.h src/main/client/render/Mesh.h src/main/client/render/Vertex.h src/main/client/render/GreenBlock.h src/main/client/render/GreenBlock.cpp src/main/constants.h src/main/client/Entity.cpp src/main/client/test.cpp src/main/threadpool.h src/main/threadpool.cpp src/main/client/event.cpp src/main/client/event.h)



