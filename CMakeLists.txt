cmake_minimum_required(VERSION 3.28.1)
project(discrete)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(DISCRETE_DEBUG)
set(CMAKE_BUILD_TYPE Debug)
include(FetchContent)

file(GLOB_RECURSE SOURCE "src/main/*")
file(GLOB_RECURSE TEST "src/test/*")
file(GLOB_RECURSE RESOURCES "resources/*")
file(GLOB_RECURSE FRAGMENT_SHADER_FILES "${CMAKE_SOURCE_DIR}/src/shader/fragment/*")
file(GLOB_RECURSE VERTEX_SHADER_FILES   "${CMAKE_SOURCE_DIR}/src/shader/vertex/*")
file(GLOB_RECURSE COMPUTE_SHADER_FILES "${CMAKE_SOURCE_DIR}/src/shader/compute/*")
set(SUBMODULE_ARTIFACTS
        ${CMAKE_SOURCE_DIR}/libglfw3.a
        ${CMAKE_SOURCE_DIR}/libgmock.a
        ${CMAKE_SOURCE_DIR}/libgmock_main.a
        ${CMAKE_SOURCE_DIR}/libgtest.a
        ${CMAKE_SOURCE_DIR}/libgtest_main.a
        ${CMAKE_SOURCE_DIR}/libspdlog.a

        ${CMAKE_SOURCE_DIR}/include/GLFW
        ${CMAKE_SOURCE_DIR}/include/data
        ${CMAKE_SOURCE_DIR}/include/glm
        ${CMAKE_SOURCE_DIR}/include/gmock
        ${CMAKE_SOURCE_DIR}/include/gtest
        ${CMAKE_SOURCE_DIR}/include/spdlog
)

find_package(Vulkan REQUIRED)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(VULKAN_LIB Vulkan REQUIRED)
    find_library(COCOA_LIB Cocoa REQUIRED)
    find_library(IOKIT_LIB IOKit REQUIRED)
    find_library(CORE_VIDEO_LIB CoreVideo REQUIRED)
    find_library(METAL_LIB Metal REQUIRED)
endif()

# Build sqlite executable for initial db generation and library for static linkage into the project
FetchContent_Declare(
        SQLITE
        URL https://www.sqlite.org/2025/sqlite-autoconf-3490200.tar.gz
        URL_HASH SHA256=5c6d8697e8a32a1512a9be5ad2b2e7a891241c334f56f8b0fb4fc6051e1652e8
)
FetchContent_MakeAvailable(SQLITE)

add_library(sqlite STATIC)
target_sources(sqlite PRIVATE "${sqlite_SOURCE_DIR}/sqlite3.c")

add_custom_command(
        OUTPUT ${sqlite_SOURCE_DIR}/sqlite3
        # as per the docs at https://github.com/sqlite/sqlite

        # apt install gcc make tcl-dev  ;#  Make sure you have all the necessary build tools
        # tar xzf sqlite.tar.gz         ;#  Unpack the source tree into "sqlite"
        # mkdir bld                     ;#  Build will occur in a sibling directory
        # cd bld                        ;#  Change to the build directory
        # ../sqlite/configure           ;#  Run the configure script
        # make sqlite3                  ;#  Builds the "sqlite3" command-line tool
        # make sqlite3.c                ;#  Build the "amalgamation" source file
        # make sqldiff                  ;#  Builds the "sqldiff" command-line tool
        # Makefile targets below this point require tcl-dev
        # make tclextension-install     ;#  Build and install the SQLite TCL extension
        # make devtest                  ;#  Run development tests
        # make releasetest              ;#  Run full release tests
        # make sqlite3_analyzer         ;#  Builds the "sqlite3_analyzer" tool
        COMMAND make sqlite3
        WORKING_DIRECTORY ${sqlite_SOURCE_DIR}
        COMMENT "Building sqlite3 target in ${sqlite_SOURCE_DIR}"
        VERBATIM
)

add_custom_target(sqlite3
        DEPENDS ${sqlite_SOURCE_DIR}/sqlite3
)

target_include_directories(
        sqlite
        PRIVATE
        ${sqlite_SOURCE_DIR}
)

add_executable(discrete)
target_sources(discrete PRIVATE ${SOURCE})

target_include_directories(
        discrete
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src/main
        ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(
    discrete
    ${VULKAN_LIB}
    ${MOLTENVK_LIB}
    ${COCOA_LIB}
    ${IOKIT_LIB}
    ${CORE_VIDEO_LIB}
    ${METAL_LIB}
    libglfw3.a
    libspdlog.a
        sqlite
)

foreach(SHADER ${FRAGMENT_SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SPIRV_OUTPUT "${CMAKE_SOURCE_DIR}/resources/shader/fragment/${SHADER_NAME}.spv")

    add_custom_command(
        OUTPUT ${SPIRV_OUTPUT}
        COMMAND glslc ${SHADER} -o ${SPIRV_OUTPUT} -fshader-stage=fragment
    )
    list(APPEND SPIRV_FILES ${SPIRV_OUTPUT})
endforeach()

foreach(SHADER ${VERTEX_SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SPIRV_OUTPUT "${CMAKE_SOURCE_DIR}/resources/shader/vertex/${SHADER_NAME}.spv")

    add_custom_command(
        OUTPUT ${SPIRV_OUTPUT}
        COMMAND glslc ${SHADER} -o ${SPIRV_OUTPUT} -fshader-stage=vertex
    )
    list(APPEND SPIRV_FILES ${SPIRV_OUTPUT})
endforeach()

foreach(SHADER ${COMPUTE_SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SPIRV_OUTPUT "${CMAKE_SOURCE_DIR}/resources/shader/compute/${SHADER_NAME}.spv")

    add_custom_command(
        OUTPUT ${SPIRV_OUTPUT}
        COMMAND glslc ${SHADER} -o ${SPIRV_OUTPUT} -fshader-stage=compute
    )
    list(APPEND SPIRV_FILES ${SPIRV_OUTPUT})
endforeach()

set(DATA_FILES)
foreach(RESOURCE_FILE ${RESOURCES})
    get_filename_component(ORIGINAL_FILENAME ${RESOURCE_FILE} NAME)
    # don't want hidden files like mac .DS_Store !!
    if(${ORIGINAL_FILENAME} MATCHES "^\\.")
        continue()
    endif()
    set(DATA_FILE ${CMAKE_SOURCE_DIR}/include/data/${ORIGINAL_FILENAME}.h)
    list(APPEND DATA_FILES ${DATA_FILE})
    message("${RESOURCE_FILE}")
    add_custom_command(
        OUTPUT ${DATA_FILE}
        COMMAND ${CMAKE_SOURCE_DIR}/scripting/embedded_data_header.sh ${RESOURCE_FILE} ${DATA_FILE}
    )
endforeach()

add_custom_command(
        OUTPUT ${SUBMODULE_ARTIFACTS}
        COMMAND scripting/deploy_modules.sh
        USES_TERMINAL
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
add_custom_target(deploy_modules ALL DEPENDS ${SUBMODULE_ARTIFACTS})
add_custom_target(build_actions
        COMMAND python3 scripting/build_actions.py
                ${CMAKE_CURRENT_SOURCE_DIR}/src/main/engine/action
                ${CMAKE_CURRENT_SOURCE_DIR}/src/main/engine/action/registration.h
        USES_TERMINAL
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
add_custom_target(compile_shaders ALL DEPENDS ${SPIRV_FILES})
add_custom_target(generate_resources ALL DEPENDS ${DATA_FILES} compile_shaders)
add_dependencies(discrete generate_resources compile_shaders deploy_modules build_actions)

foreach(TEST_FILE ${TEST})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    execute_process(
        COMMAND /usr/local/bin/python3 ${CMAKE_CURRENT_SOURCE_DIR}/scripting/parse_link_deps.py ${TEST_FILE} ${CMAKE_CURRENT_LIST_DIR}
        OUTPUT_VARIABLE LINK_DEPS
        ECHO_OUTPUT_VARIABLE
    )

    set(TEST_SOURCES ${LINK_DEPS} ${TEST_FILE})

    add_executable(${TEST_NAME} ${TEST_SOURCES})

    target_link_libraries(${TEST_NAME} gtest.a)

    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()