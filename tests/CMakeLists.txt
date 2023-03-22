add_executable(use_tests test_use.cpp)
target_link_libraries(use_tests PRIVATE UrbanSensingEngine)

add_test(NAME COCOTest00 COMMAND use_tests)