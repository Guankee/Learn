cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
get_filename_component(SOURCE_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
# project(${NAME})
# CMAKE_CURRENT_SOURCE_DIR 当前处理的cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/release)
include_directories(
    ${CMAKE_SOURCE_DIR}/Libs/glad3.3/include
    ${CMAKE_SOURCE_DIR}/Libs/glfw/include
    ${CMAKE_SOURCE_DIR}/Libs/glm-0.9.9.8/glm
    ${CMAKE_SOURCE_DIR}/common
)
file(GLOB SOURCE_FILE 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
${CMAKE_SOURCE_DIR}/common/*.h
${CMAKE_SOURCE_DIR}/common/*.c
)

set(Libs 
${CMAKE_SOURCE_DIR}/Libs/glfw/lib-vc2019/glfw3_mt.lib
${CMAKE_SOURCE_DIR}/Libs/glfw/lib-vc2019/glfw3.lib
${CMAKE_SOURCE_DIR}/Libs/glfw/lib-vc2019/glfw3dll.lib
opengl32.lib
)

add_executable(
    ${SOURCE_DIR_NAME}
    ${SOURCE_FILE}
) 

target_link_libraries( ${SOURCE_DIR_NAME}   ${Libs})