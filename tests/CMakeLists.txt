add_executable(use_tests test_use.cpp)
target_link_libraries(use_tests PRIVATE UrbanSensingEngine COCO PlExA oRatioSolver ratioCore RiDDLe SeMiTONE json ${CMAKE_THREAD_LIBS_INIT} PahoMqttCpp::paho-mqttpp3-static clips mongo::bsoncxx_shared mongo::mongocxx_shared)

add_test(NAME COCOTest00 COMMAND use_tests)