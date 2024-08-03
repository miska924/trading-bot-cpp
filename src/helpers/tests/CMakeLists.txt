add_executable(test_function_queue test_function_queue.cpp)

target_link_libraries(
    test_function_queue
    GTest::gtest_main
    function_queue
)

gtest_discover_tests(test_function_queue)
