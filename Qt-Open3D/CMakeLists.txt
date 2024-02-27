cmake_minimum_required(VERSION 3.10)
project(Qt-3d)

set( CMAKE_AUTOMOC ON )
# set( CMAKE_AUTORCC ON )
# FIXME Eventually turn this on when we've completed the move to targets
#set( CMAKE_AUTOUIC ON )
# https://cmake.org/cmake/help/v3.0/policy/CMP0071.html
# 设置 CMP0071 政策为 NEW 是确保 Windows 系统上的 Qt 项目在 CMake 3.10 及之后的版本中正常运行的必要步骤
cmake_policy(SET CMP0071 NEW)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/debug)

find_package( Qt5
	COMPONENTS
	REQUIRED
  Core
  Widgets
  Gui
  OpenGL
)
set(Open3D_DIR "D:/Learn/Libs/Open3D0.18-CUDA11.6/intall/CMake")
find_package(Open3D REQUIRED)
include_directories(${Open3D_INCLUDE_DIRS})
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/ui)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/qrc)
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

include_directories( ${CMAKE_SOURCE_DIR}/Libs/glew-2.1.0/include )
set(GLEW_Lib ${CMAKE_SOURCE_DIR}/Libs/glew-2.1.0/lib/Release/x64/glew32.lib)

file( GLOB header_list ${CMAKE_CURRENT_SOURCE_DIR}/*.h  )
file( GLOB source_list ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )
file( GLOB ui_list ${CMAKE_CURRENT_SOURCE_DIR}/ui/*.ui )
file( GLOB qrc_list ${CMAKE_CURRENT_SOURCE_DIR}/qrc/*.qrc )

qt5_wrap_ui( generated_ui_list ${ui_list} )
# # 处理资源文件，生成 C++ 代码
# qt5_add_resources( generated_qrc_list ${qrc_list} )

add_executable(${PROJECT_NAME} ${header_list} 
${source_list} ${generated_ui_list} ${generated_qrc_list})

target_link_libraries(${PROJECT_NAME} ${Open3D_LIBRARIES} ${GLEW_Lib} Qt5::OpenGL Qt5::Core
Qt5::Gui Qt5::Widgets  )
