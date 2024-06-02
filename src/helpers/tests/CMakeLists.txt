add_executable(test_date_time test_date_time.cpp)

target_link_libraries(
    test_date_time
    gtest_main
    date_time
)

gtest_discover_tests(test_date_time)
