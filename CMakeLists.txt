cmake_minimum_required(VERSION 3.0.0)
project(urban-sensing-engine VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)
include(GenerateExportHeader)

find_package(PahoMqttCpp REQUIRED)

include(CTest)
enable_testing()

add_subdirectory(oRatio/smt/json)
add_executable(${PROJECT_NAME} main.cpp src/urban_sensing_engine.cpp src/rule.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PahoMqttCpp::paho-mqttpp3 json)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
