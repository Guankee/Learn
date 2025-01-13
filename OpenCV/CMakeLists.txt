cmake_minimum_required(VERSION 3.20)
project(Opencv)


# cmake_policy(SET CMP0071 NEW)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/debug)

set(OpenCV_DIR "D:\\3D_Libs\\opencv-4.8.1\\install\\x64\\vc16\\lib")
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
message(${OpenCV_INCLUDE_DIRS})

include_directories("D:\\3D_Libs\\opencv-4.8.1\\install\\include")
add_executable(HelloCV main.cpp)
target_link_libraries(HelloCV ${OpenCV_LIBRARIES})

add_executable(aruco aruco.cpp)
target_link_libraries(aruco ${OpenCV_LIBRARIES})
target_include_directories(aruco PRIVATE ${OpenCV_INCLUDE_DIRS})