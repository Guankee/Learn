cmake_minimum_required(VERSION 3.20)
project(Open3D)


# cmake_policy(SET CMP0071 NEW)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/debug)

list( APPEND CMAKE_MODULE_PATH "D:/Learn/Libs/Open3D0.18-CUDA11.6/intall/CMake" )
set(Open3D_DIR "D:/Learn/Libs/Open3D0.18-CUDA11.6/intall/CMake")


find_package(Open3D REQUIRED)
include_directories(${Open3D_INCLUDE_DIRS})
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

add_executable(open3d_test open3d_test.cpp)
target_link_libraries(open3d_test   ${Open3D_LIBRARIES})