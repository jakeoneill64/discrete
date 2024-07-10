cmake_minimum_required(VERSION 3.28.1)
project(discrete)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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

file(GLOB_RECURSE RESOURCE "resources/*")

set(DATA_FILES)
foreach(RESOURCE_FILE ${RESOURCE})
    get_filename_component(RESOURCE_FILE_NAME ${RESOURCE_FILE} NAME_WE)
    set(DATA_FILE ${CMAKE_SOURCE_DIR}/include/${RESOURCE_FILE_NAME}.dat)
    list(APPEND DATA_FILES ${DATA_FILE})
    add_custom_command(
        OUTPUT ${DATA_FILE}
        COMMAND xxd -i ${RESOURCE_FILE} ${DATA_FILE}
    )
endforeach()

include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src/main)

file(GLOB_RECURSE SOURCE "src/main/*")
file(GLOB_RECURSE TEST "src/test/*")


add_executable(discrete ${SOURCE})

add_custom_target(GenerateResources ALL DEPENDS ${DATA_FILES})
add_dependencies(discrete GenerateResources)

# Create a separate test executable for each test source
foreach(TEST_FILE ${TEST})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    execute_process(
        COMMAND /usr/local/bin/python3 ${CMAKE_CURRENT_SOURCE_DIR}/scripting/parse_link_deps.py ${TEST_FILE} ${CMAKE_CURRENT_LIST_DIR}
        OUTPUT_VARIABLE LINK_DEPS
        ECHO_OUTPUT_VARIABLE
    )

    set(TEST_SOURCES ${LINK_DEPS} ${TEST_FILE})

    add_executable(${TEST_NAME} ${TEST_SOURCES})

    # Link the test executable with the main project code and testing framework
    target_link_libraries(${TEST_NAME} gtest.a)

    # Add a test
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()


