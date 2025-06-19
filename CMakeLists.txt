cmake_minimum_required(VERSION 3.21)
project(USPE VERSION 1.0.0 LANGUAGES CXX)

include(CTest)
enable_testing()

set(COCO_NAME "USPE" CACHE STRING "The Urban Sensing and Planning Engine (USPE) application name" FORCE)
set(BUILD_MONGODB ON CACHE BOOL "Build MongoDB drivers" FORCE)
set(BUILD_COCO_SERVER ON CACHE BOOL "Build the CoCo server" FORCE)
set(BUILD_AUTH ON CACHE BOOL "Build CoCo authentication" FORCE)
set(LOGGING_LEVEL "TRACE" CACHE STRING "Logging level for CoCo" FORCE)

add_subdirectory(extern/coco)

add_executable(USPE src/main.cpp src/uspe.cpp src/uspe_db.cpp src/uspe_server.cpp)
add_dependencies(USPE CoCo)
target_compile_features(USPE PUBLIC cxx_std_17)
target_include_directories(USPE PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(USPE PUBLIC CoCo)
setup_sanitizers(USPE)

set(CPACK_PROJECT_NAME USPE)
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
