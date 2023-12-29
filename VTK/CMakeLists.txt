cmake_minimum_required(VERSION 3.10)
project(VTK)


cmake_policy(SET CMP0071 NEW)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/debug)


# 添加 Open3D 的安装目录到 CMAKE_MODULE_PATH
list( APPEND CMAKE_MODULE_PATH "D:/3D_Libs/VTK-9.1.0/install/lib/cmake/vtk-9.1/" )
list( APPEND CMAKE_MODULE_PATH "E:/Medical_ae/trunk/Libs/Open3D-0.17.0-CUDA11.6/CMake/" )
set(Open3D_DIR "E:/Medical_ae/trunk/Libs/Open3D-0.17.0-CUDA11.6/CMake/")
# include(vtk-config)
find_package(VTK REQUIRED)
include_directories(${VTK_INCLUDE_DIRS})

find_package(Open3D REQUIRED)
include_directories(${Open3D_INCLUDE_DIRS})
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )



file( GLOB header_list ${CMAKE_CURRENT_SOURCE_DIR}/*.h  )
file( GLOB source_list ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )


add_executable(${PROJECT_NAME} ${header_list} 
${source_list} )

target_link_libraries(${PROJECT_NAME}   ${VTK_LIBRARIES} ${Open3D_LIBRARIES})
