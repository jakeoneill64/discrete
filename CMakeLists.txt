cmake_minimum_required(VERSION 3.28.1)
project(discrete)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
link_directories(lib)
add_compile_definitions(DISCRETE_DEBUG)
set(CMAKE_BUILD_TYPE Debug)

add_executable(discrete)

find_package(Vulkan REQUIRED)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(VULKAN_LIB Vulkan REQUIRED)
    find_library(COCOA_LIB Cocoa REQUIRED)
    find_library(IOKIT_LIB IOKit REQUIRED)
    find_library(CORE_VIDEO_LIB CoreVideo REQUIRED)
    find_library(METAL_LIB Metal REQUIRED)
endif()

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
)

file(GLOB_RECURSE RESOURCES "resources/*")
file(GLOB_RECURSE FRAGMENT_SHADER_FILES "${CMAKE_SOURCE_DIR}/src/shader/fragment/*")
file(GLOB_RECURSE VERTEX_SHADER_FILES   "${CMAKE_SOURCE_DIR}/src/shader/vertex/*")
file(GLOB_RECURSE COMPUTE_SHADER_FILES "${CMAKE_SOURCE_DIR}/src/shader/compute/*")

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


execute_process(
        COMMAND /usr/local/bin/python3 ${CMAKE_SOURCE_DIR}/scripting/build_actions.py ${CMAKE_CURRENT_SOURCE_DIR}/src/main/engine/action ${CMAKE_CURRENT_SOURCE_DIR}/src/main/engine/action/registration.h
        OUTPUT_VARIABLE LINK_DEPS
)

#execute_process(
#        COMMAND python ${CMAKE_SOURCE_DIR}/scripting/deploy_modules.py
#)

#add_custom_target(deploy_modules
#    COMMAND scripting/deploy_modules.sh
#    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#)

# TODO
# sort out rebuild modules on every launch
# database file (and other application state) should go to /var/discrete
# there should be a target to run the db scripts at build time.

include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src/main ${Vulkan_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCE "src/main/*")
file(GLOB_RECURSE TEST "src/test/*")
target_sources(discrete PRIVATE ${SOURCE})

add_custom_target(compile_shaders ALL DEPENDS ${SPIRV_FILES})
add_custom_target(generate_resources ALL DEPENDS ${DATA_FILES} compile_shaders)
add_dependencies(discrete generate_resources compile_shaders)


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