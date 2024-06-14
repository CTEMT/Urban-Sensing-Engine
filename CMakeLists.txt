cmake_minimum_required(VERSION 3.5.0)
project(UrbanSensingAndPlanningEngine VERSION 0.2.0 LANGUAGES CXX)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined")

include(CTest)
enable_testing()

add_subdirectory(extern/coco)
add_subdirectory(extern/network)

set(DASHBOARD_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/../dashboard" CACHE STRING "The Urban Sensing & Planning Engine Dashboard Client folder")
message(STATUS "Dashboard folder: ${DASHBOARD_FOLDER}")

configure_file(.env.in ${DASHBOARD_FOLDER}/.env.local @ONLY)
execute_process(COMMAND npm install WORKING_DIRECTORY ${DASHBOARD_FOLDER})
execute_process(COMMAND npm run build WORKING_DIRECTORY ${DASHBOARD_FOLDER})

add_executable(UrbanSensingAndPlanningEngine src/main.cpp src/uspe_server.cpp src/uspe_db.cpp)
target_include_directories(UrbanSensingAndPlanningEngine PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
add_dependencies(UrbanSensingAndPlanningEngine CoCo ratioNet)
target_link_libraries(UrbanSensingAndPlanningEngine CoCo ratioNet)
target_compile_definitions(UrbanSensingAndPlanningEngine PRIVATE DASHBOARD_FOLDER="${DASHBOARD_FOLDER}")

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
