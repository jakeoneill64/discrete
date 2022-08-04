cmake_minimum_required(VERSION 3.21)
project(mroneillscubes)

set(CMAKE_CXX_STANDARD 23)

add_executable(mroneillscubes client/Main.cpp server/Server.cpp server/Server.h client/render/Window.h client/render/Window.cpp server/business/World.h server/business/World.cpp server/business/Voxel.cpp server/business/Voxel.h)

target_link_libraries(mroneillscubes lib/libglad.a lib/libglfw3.a lib/libglm_static.a lib/libjsoncpp.a)
include_directories(${CMAKE_SOURCE_DIR}/include)