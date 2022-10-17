cmake_minimum_required(VERSION 3.0.0)
project(UrbanSensingEngine VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

find_package(Threads REQUIRED)
find_package(Boost REQUIRED)
find_package(PahoMqttCpp REQUIRED)

include(CTest)
enable_testing()

set(CLIPS_INCLUDE_DIR /usr/local/include/clips CACHE PATH "CLPIS include directory")
set(CLIPS_LIB_DIR /usr/local/lib CACHE PATH "CLPIS library directory")

set(USE_MQTT_HOST "tcp://127.0.0.1" CACHE STRING "The Urban Sensing & Planning Engine MQTT Host")
set(USE_MQTT_PORT "1883" CACHE STRING "The Urban Sensing & Planning Engine MQTT Port")
set(USE_DASHBOARD_HOST "127.0.0.1" CACHE STRING "The Urban Sensing & Planning Engine Dashboard Host")
set(USE_DASHBOARD_PORT "8080" CACHE STRING "The Urban Sensing & Planning Engine Dashboard Port")
set(LOCATION Catania CACHE STRING "Dashboard location")
set_property(CACHE LOCATION PROPERTY STRINGS Matera Catania)

message(STATUS "Location:               ${LOCATION}")
if(LOCATION STREQUAL Matera)
    set(USE_ROOT "CTE-MT" CACHE STRING "The Urban Sensing & Planning Engine root")
    set(USE_DASHBOARD_LAT "40.666379" CACHE STRING "The Urban Sensing & Planning Engine Map Latitude")
    set(USE_DASHBOARD_LNG "16.604320" CACHE STRING "The Urban Sensing & Planning Engine Map Longitude")
elseif(LOCATION STREQUAL Catania)
    set(USE_ROOT "UISH" CACHE STRING "The Urban Sensing & Planning Engine root")
    set(USE_DASHBOARD_LAT "37.5078772" CACHE STRING "The Urban Sensing & Planning Engine Map Latitude")
    set(USE_DASHBOARD_LNG "15.0830304" CACHE STRING "The Urban Sensing & Planning Engine Map Longitude")
endif()

option(SOLVING_MONITORING "Notify the GUI of any changes during the resolution process" OFF)

message(STATUS "Solving monitoring:     ${SOLVING_MONITORING}")
if(SOLVING_MONITORING)
    target_compile_definitions(${PROJECT_NAME} PRIVATE SOLVING_MONITORING)
endif()

set(COMPUTE_NAMES ON CACHE BOOL "Computes the objects' names" FORCE)

add_subdirectory(extern/plexa)

file(GLOB URBAN_SENSING_ENGINE_SOURCES src/*.cpp)
file(GLOB URBAN_SENSING_ENGINE_HEADERS include/*.h)

add_executable(${PROJECT_NAME} ${URBAN_SENSING_ENGINE_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/extern/crow $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> $<INSTALL_INTERFACE:> ${Boost_INCLUDE_DIRS} ${CLIPS_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${CLIPS_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE PlExA oRatioSolver ratioCore RiDDLe SeMiTONE json ${CMAKE_THREAD_LIBS_INIT} PahoMqttCpp::paho-mqttpp3-static clips)

target_compile_definitions(${PROJECT_NAME} PRIVATE USE_ROOT="${USE_ROOT}" USE_MQTT_HOST="${USE_MQTT_HOST}" USE_MQTT_PORT="${USE_MQTT_PORT}" USE_DASHBOARD_HOST="${USE_DASHBOARD_HOST}" USE_DASHBOARD_PORT=${USE_DASHBOARD_PORT} USE_DASHBOARD_LAT=${USE_DASHBOARD_LAT} USE_DASHBOARD_LNG=${USE_DASHBOARD_LNG})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

install(
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(
    FILES ${RATIO_CORE_HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_export.h
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)
configure_package_config_file(src/${PROJECT_NAME}Config.cmake.in ${PROJECT_NAME}Config.cmake INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME} PATH_VARS CMAKE_INSTALL_INCLUDEDIR)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake VERSION 1.0.0 COMPATIBILITY SameMajorVersion)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)