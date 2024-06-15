add_executable(test_ema_feature test_ema_feature.cpp)

target_link_libraries(
  test_ema_feature
  GTest::gtest_main
  market
  backtest_market
  feature
  ema_feature
  vector_view
)

gtest_discover_tests(test_ema_feature)
