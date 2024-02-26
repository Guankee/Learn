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
add_executable(HelloITK main.cpp)
target_link_libraries(HelloITK ${ITK_LIBRARIES})
