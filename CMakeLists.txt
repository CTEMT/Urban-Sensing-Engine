cmake_minimum_required(VERSION 3.21)
project(USPE VERSION 1.0.0 LANGUAGES CXX)

include(CTest)
enable_testing()

set(COCO_NAME "USPE" CACHE STRING "The Urban Sensing and Planning Engine (USPE) application name")

add_subdirectory(extern/coco)

add_executable(USPE src/main.cpp)
add_dependencies(USPE CoCo)
target_compile_features(USPE PUBLIC cxx_std_17)
target_include_directories(USPE PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(USPE PUBLIC CoCo)
setup_sanitizers(USPE)

set(CPACK_PROJECT_NAME USPE)
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
