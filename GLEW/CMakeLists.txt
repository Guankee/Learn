cmake_minimum_required(VERSION 3.20)
project(glew)


# cmake_policy(SET CMP0071 NEW)
# set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/debug)

# list( APPEND CMAKE_MODULE_PATH "D:/Learn/Libs/Open3D0.18-CUDA11.6/intall/CMake" )
set(GLEW_DIR "D:/3Libs/glew-20220402/glew-2.2.0/install/lib/cmake/glew")


find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

# include_directories(${CMAKE_SOURCE_DIR})


add_executable(glew_test glew_test.cpp)
target_link_libraries(glew_test  GLEW::glew)