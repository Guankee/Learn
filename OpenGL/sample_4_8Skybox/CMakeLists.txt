cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
get_filename_component(SOURCE_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
# project(${NAME})
# CMAKE_CURRENT_SOURCE_DIR 当前处理的cmake

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/debug)

# 添加 Assimp 的 CMake 配置文件
# ${CMAKE_SOURCE_DIR}/Libs/assimp5.0.1/lib/cmake/assimp-5.0
# set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/Libs/assimp5.0.1/lib/cmake/assimp-5.0)
# set(CMAKE_PREFIX_PATH D:/ThreePart/ass_install/lib/cmake/assimp-5.0)
# set(CMAKE_SYSTEM_MODULE_PATH "D:\\ThreePart\\ass_install\\lib\\cmake\\assimp-5.0")
# find_package(assimp REQUIRED)

include_directories(
    ${CMAKE_SOURCE_DIR}/Libs/glad3.3/include
    ${CMAKE_SOURCE_DIR}/Libs/glfw/include
    ${CMAKE_SOURCE_DIR}/Libs/glm-0.9.9.8/glm
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/Libs/assimp5.0.1/include
    # ${ASSIMP_INCLUDE_DIR}
)
# message("ASSIMP_INCLUDE_DIR--" ${ASSIMP_INCLUDE_DIR})


file(GLOB SOURCE_FILE 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
${CMAKE_SOURCE_DIR}/common/*.h
${CMAKE_SOURCE_DIR}/common/*.c

)
# set(Libs
# ${CMAKE_SOURCE_DIR}/Libs/glfw/lib-vc2019/glfw3_mt.lib
# ${CMAKE_SOURCE_DIR}/Libs/glfw/lib-vc2019/glfw3.lib
# ${CMAKE_SOURCE_DIR}/Libs/glfw/lib-vc2019/glfw3dll.lib
# opengl32.lib
# )
set(Libs_Debug
    ${CMAKE_SOURCE_DIR}/Libs/glfw/lib-vc2019/glfw3_mt.lib
    ${CMAKE_SOURCE_DIR}/Libs/glfw/lib-vc2019/glfw3.lib
    ${CMAKE_SOURCE_DIR}/Libs/glfw/lib-vc2019/glfw3dll.lib
    opengl32.lib
    ${CMAKE_SOURCE_DIR}/Libs/assimp5.0.1/lib/assimp-vc142-mtd.lib

)

set(Libs_Release
    ${CMAKE_SOURCE_DIR}/Libs/glfw/lib-vc2019/glfw3_mt.lib
    ${CMAKE_SOURCE_DIR}/Libs/glfw/lib-vc2019/glfw3.lib
    ${CMAKE_SOURCE_DIR}/Libs/glfw/lib-vc2019/glfw3dll.lib
    opengl32.lib
    ${CMAKE_SOURCE_DIR}/Libs/assimp5.0.1/lib/assimp-vc142-mt.lib
)

add_executable(
    ${SOURCE_DIR_NAME}
    ${SOURCE_FILE}
) 
add_custom_command(
  PRE_BUILD
  TARGET ${SOURCE_DIR_NAME} # 这里是需要提前执行的 target
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/bin/release
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/bin/debug
)

# if debug 这种命令不行
add_custom_command(TARGET ${SOURCE_DIR_NAME} POST_BUILD
 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/Libs/assimp5.0.1/bin/assimp-vc142-mtd.dll 
${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})

  
add_custom_command(TARGET  ${SOURCE_DIR_NAME}  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/Libs/assimp5.0.1/bin/assimp-vc142-mt.dll 
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})

# target_link_libraries(
#     ${SOURCE_DIR_NAME}
#     ${Libs}
#     ${ASSIMP_LIBRARIES}
# )


# 判断是否为 debug 模式


target_link_libraries(
    ${SOURCE_DIR_NAME}
    $<$<CONFIG:Debug>:${Libs_Debug}>
    $<$<CONFIG:Release>:${Libs_Release}>
)

# message("ASSIMP_LIBRARIES--" ${ASSIMP_LIBRARIES})

