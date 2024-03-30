include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(test_backtesting test_backtesting.cpp)
target_link_libraries(test_backtesting gtest_main backtesting)
include(GoogleTest)
gtest_discover_tests(test_backtesting)
