cmake_minimum_required(VERSION 3.20)
project(PCH)

set(CMAKE_CXX_STANDARD 17)

add_executable(MyApp main.cpp pch.cpp)
# 告诉 MSVC 使用 pch
target_precompile_headers(MyApp PRIVATE pch.h)


# MSVC-specific optimization
if(MSVC)
  target_compile_options(MyApp PRIVATE /W4 /MP)
endif()
