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
    link_libraries(${OPEN_GL_LIB} ${COCOA_LIB} ${IOKIT_LIB} ${CORE_VIDEO_LIB})
endif()

link_libraries(libglad.a libglfw3.a libspdlog.a)

include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src/main)

file(GLOB_RECURSE SOURCE "src/main/*")
file(GLOB_RECURSE TEST "src/test/*")

add_executable(discrete ${SOURCE})

# Create a separate test executable for each test source
foreach(TEST_FILE ${TEST})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(build_${TEST_NAME} ${TEST_FILE})

    # Link the test executable with the main project code and testing framework
    target_link_libraries(build_${TEST_NAME})


    add_custom_target(${TEST_NAME} COMMAND ./${TEST_NAME})

    # Add a test
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()


