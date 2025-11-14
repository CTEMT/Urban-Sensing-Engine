cmake_minimum_required(VERSION 3.21)
project(USPE VERSION 1.0.0 LANGUAGES CXX)

include(CTest)
enable_testing()

set(COCO_NAME "USPE" CACHE STRING "The Urban Sensing and Planning Engine (USPE)  CoCo Application Name" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "Build tests" FORCE)
set(BUILD_MONGODB ON CACHE BOOL "Build MongoDB connection" FORCE)
set(BUILD_COCO_SERVER ON CACHE BOOL "Build the CoCo server" FORCE)
set(LOGGING_LEVEL "TRACE" CACHE STRING "Logging level for CoCo" FORCE)

add_subdirectory(extern/coco)

set(USPE_CONFIG_TYPES types/Location.json types/POI.json types/Church.json)
set(USPE_CONFIG_RULES rules/uspe.clp)
coco_config(USPE "${USPE_CONFIG_TYPES}" "${USPE_CONFIG_RULES}")

add_executable(uspe src/main.cpp src/uspe.cpp src/uspe_db.cpp src/uspe_server.cpp)
target_compile_features(uspe PUBLIC cxx_std_17)
target_include_directories(uspe PUBLIC "${USPE_CONFIG_INCLUDE_DIR}" $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
add_dependencies(uspe CoCo USPE)
target_link_libraries(uspe PUBLIC CoCo)

setup_sanitizers(uspe)

set(CPACK_PROJECT_NAME uspe)
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
