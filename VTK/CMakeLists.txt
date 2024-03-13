cmake_minimum_required(VERSION 3.10)
project(VTK)


cmake_policy(SET CMP0071 NEW)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/debug)



set(Open3D_DIR "E:/Downloads/Open3D-2/Open3D/CMake/")
set(VTK_DIR "E:/Downloads/PCL-1.14.0/3rdParty/vtk/lib/cmake/vtk-9.3/")
# include(vtk-config)
find_package(VTK REQUIRED )
include_directories(${VTK_INCLUDE_DIRS})

find_package(Open3D REQUIRED)
include_directories(${Open3D_INCLUDE_DIRS})
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )



# file( GLOB header_list ${CMAKE_CURRENT_SOURCE_DIR}/*.h  )
# file( GLOB source_list ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )


# add_executable(vtk1 main.cpp)
# target_link_libraries(vtk1    ${VTK_LIBRARIES} ${Open3D_LIBRARIES})

add_executable(3d2vtk 3D2vtk.cpp)
target_link_libraries(3d2vtk  ${VTK_LIBRARIES} ${Open3D_LIBRARIES})
add_executable(open3d open3d.cpp)
target_link_libraries(open3d   ${Open3D_LIBRARIES})