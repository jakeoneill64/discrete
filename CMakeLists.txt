cmake_minimum_required(VERSION 3.21)
project(mroneillscubes)

set(CMAKE_CXX_STANDARD 23)

add_executable(mroneillscubes client/Main.cpp server/Server.cpp server/Server.h client/render/Window.h client/render/Window.cpp server/business/World.h server/business/World.cpp server/business/Voxel.cpp server/business/Voxel.h)
