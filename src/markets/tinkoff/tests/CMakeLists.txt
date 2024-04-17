add_executable(test_tinkoff_api test_tinkoff_api.cpp)

target_link_libraries(
    test_tinkoff_api
    gtest_main
    tinkoff_api
)

gtest_discover_tests(test_tinkoff_api)
