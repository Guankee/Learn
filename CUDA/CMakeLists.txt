# cmake_minimum_required(VERSION 3.20)
# project(CUDA_TEST)
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/release)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/debug)
# find_package(CUDA REQUIRED)

# message(STATUS "cuda version: " ${CUDA_VERSION_STRING})
# include_directories(${CUDA_INCLUDE_DIRS})

# cuda_add_executable(cuda_test cuda_test.cu)
# target_link_libraries(cuda_test ${CUDA_LIBRARIES})

cmake_minimum_required(VERSION 3.10)
project(CUDA_TEST LANGUAGES CUDA CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/debug)
include(CheckLanguage)
check_language(CUDA)

add_executable(cuda_test cuda_test.cu)
