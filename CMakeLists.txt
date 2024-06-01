cmake_minimum_required(VERSION 3.5.0)
project(UrbanSensingAndPlanningEngine VERSION 0.2.0 LANGUAGES CXX)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined")

include(CTest)
enable_testing()

add_subdirectory(extern/coco)
add_subdirectory(extern/network)

add_executable(UrbanSensingAndPlanningEngine src/main.cpp)
target_include_directories(UrbanSensingAndPlanningEngine PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/mongo> ${LIBMONGOCXX_INCLUDE_DIR} ${LIBBSONCXX_INCLUDE_DIR} ${CLIPS_INCLUDE_DIR})
add_dependencies(UrbanSensingAndPlanningEngine CoCo ratioNet)

if(MSVC)
    target_compile_options(CoCo PRIVATE /W4)
else()
    target_compile_options(CoCo PRIVATE -Wall -Wextra -Wpedantic)
    if (ENABLE_COVERAGE)
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(CoCo PRIVATE --coverage)
            target_link_libraries(CoCo PUBLIC gcov)
        endif()
    endif()
endif()

set(CPACK_PROJECT_NAME CoCo)
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
