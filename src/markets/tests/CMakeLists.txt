add_executable(test_dummy_market test_dummy_market.cpp)
add_executable(test_backtest_market test_backtest_market.cpp)
add_executable(test_tinkoff_market test_tinkoff_market.cpp)

target_link_libraries(
  test_dummy_market
  gtest_main
  dummy_market
  market
  vector_view
)
target_link_libraries(
  test_backtest_market
  gtest_main
  backtest_market
  market
  vector_view
)
target_link_libraries(
  test_tinkoff_market
  gtest_main
  tinkoff_market
  market
  vector_view
  grpc++
)

gtest_discover_tests(test_dummy_market)
gtest_discover_tests(test_backtest_market)
