cmake_minimum_required(VERSION 3.20)
project(HelloITK)


# cmake_policy(SET CMP0071 NEW)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/../bin/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/../bin/debug)

#  list( APPEND CMAKE_MODULE_PATH "D:/3Libs/InsightToolkit-5.3.0/install-re/lib/cmake/ITK-5.3" )
# set(ITK_DIR "D:/3Libs/InsightToolkit-5.3.0/install-re/lib/cmake/ITK-5.3")
#  list( APPEND CMAKE_MODULE_PATH "D:/3Libs/InsightToolkit-5.3.0/install-share/lib/cmake/ITK-5.3" )
set(ITK_DIR "D:/3Libs/InsightToolkit-5.3.0/install-share/lib/cmake/ITK-5.3")

# include_directories(${ITK_INCLUDE_DIRS})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

# add_executable(HelloITK main.cpp)
# target_link_libraries(HelloITK ${ITK_LIBRARIES})
# 获取当前目录下的所有 .cpp 文件
file(GLOB cpp_files "*.cpp" "*.cxx")

# 遍历所有 .cpp 文件
foreach(cpp_file ${cpp_files})
    # 获取不带扩展名的文件名
    get_filename_component(executable_name ${cpp_file} NAME_WE)
    # 为每个 .cpp 文件创建可执行文件
    add_executable(${executable_name} ${cpp_file})
    # 链接 ITK 库
    target_link_libraries(${executable_name} ${ITK_LIBRARIES})
endforeach()