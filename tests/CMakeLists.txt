include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(test_market test_market.cpp)
add_executable(test_backtest_market test_backtest_market.cpp)
add_executable(test_strategy test_strategy.cpp)

target_link_libraries(
  test_market
  gtest_main
  market
)
target_link_libraries(
  test_backtest_market
  gtest_main
  backtest_market
  market
)
target_link_libraries(
  test_strategy
  gtest_main
  market
  backtest_market
  strategy
)

include(GoogleTest)

gtest_discover_tests(test_market)
gtest_discover_tests(test_backtest_market)
gtest_discover_tests(test_strategy)
