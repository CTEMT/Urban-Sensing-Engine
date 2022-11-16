add_executable(use_tests test_use.cpp)
target_include_directories(use_tests PUBLIC)
target_link_libraries(use_tests PRIVATE json PahoMqttCpp::paho-mqttpp3-static)
target_compile_definitions(use_tests PRIVATE USE_MQTT_HOST="${USE_MQTT_HOST}" USE_MQTT_PORT="${USE_MQTT_PORT}" USE_MONGODB_HOST="${USE_MONGODB_HOST}" USE_MONGODB_PORT="${USE_MONGODB_PORT}")

if(LOCATION STREQUAL Matera)
    target_compile_definitions(use_tests PRIVATE
        USE_ROOT="CTE-MT"
        USE_DASHBOARD_LAT="40.666379"
        USE_DASHBOARD_LNG="16.604320")
elseif(LOCATION STREQUAL Catania)
    target_compile_definitions(use_tests PRIVATE
        USE_ROOT="UISH"
        USE_DASHBOARD_LAT="37.507053"
        USE_DASHBOARD_LNG="15.086007")
endif()

add_executable(use_db test_db.cpp)
target_include_directories(use_db PUBLIC)
target_link_libraries(use_db PRIVATE json mongo::bsoncxx_shared mongo::mongocxx_shared)
target_compile_definitions(use_db PRIVATE USE_MONGODB_HOST="${USE_MONGODB_HOST}" USE_MONGODB_PORT="${USE_MONGODB_PORT}")

if(LOCATION STREQUAL Matera)
    target_compile_definitions(use_db PRIVATE USE_ROOT="CTE-MT")
elseif(LOCATION STREQUAL Catania)
    target_compile_definitions(use_db PRIVATE USE_ROOT="UISH")
endif()

add_test(NAME Use_00 COMMAND use_tests)
add_test(NAME Use_DB_00 COMMAND use_db)
