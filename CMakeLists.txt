cmake_minimum_required(VERSION 3.21)
project(mroneillscubes)

set(CMAKE_CXX_STANDARD 23)
link_directories(lib)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(COCOA_LIB Cocoa REQUIRED)
    find_library(OPEN_GL_LIB OpenGl REQUIRED)
    find_library(IOKIT_LIB IOKit REQUIRED)
    find_library(CORE_VIDEO_LIB CoreVideo REQUIRED)
    link_libraries(libboost_unit_test_framework.dylib ${OPEN_GL_LIB} ${COCOA_LIB} ${IOKIT_LIB} ${CORE_VIDEO_LIB})
endif()

link_libraries(libglad.a libglfw3.a libjsoncpp.a libspdlog.a)

include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)


add_executable(mroneillscubes src/client/Main.cpp src/server/Server.cpp src/server/Server.h src/server/business/World.h src/server/business/World.cpp src/server/business/Voxel.cpp src/server/business/Voxel.h src/client/render/RenderConfiguration.h src/client/render/RenderClient.h src/client/render/RenderClient.cpp src/client/render/RenderConfiguration.cpp src/client/render/Camera.h src/client/render/Camera.cpp src/client/render/Shader.h src/client/render/Shader.cpp src/Log.h src/Log.cpp src/client/render/Mesh.h src/client/render/Mesh.cpp src/client/Client.h src/client/Client.cpp src/client/input/InputConfiguration.h src/client/input/InputConfiguration.cpp src/client/input/InputManager.h src/client/input/InputManager.cpp src/util/Event.h src/util/Event.cpp src/Configuration.h)



