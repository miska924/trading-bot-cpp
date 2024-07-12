add_executable(test_http_client test_http_client.cpp)

target_link_libraries(
    test_http_client
    http_client
    cpr::cpr
    jsoncpp
    GTest::gtest_main
)

gtest_discover_tests(test_http_client)
