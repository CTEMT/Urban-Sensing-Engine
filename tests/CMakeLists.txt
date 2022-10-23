add_executable(use_tests test_use.cpp)
target_include_directories(use_tests PUBLIC)
target_link_libraries(use_tests PRIVATE json PahoMqttCpp::paho-mqttpp3-static)
target_compile_definitions(use_tests PRIVATE USE_ROOT="${USE_ROOT}" USE_MQTT_HOST="${USE_MQTT_HOST}" USE_MQTT_PORT="${USE_MQTT_PORT}")

add_test(NAME Use_00 COMMAND use_tests)
