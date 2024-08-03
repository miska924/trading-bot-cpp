add_executable(test_ema_feature test_ema_feature.cpp)
add_executable(test_sma_feature test_sma_feature.cpp)
add_executable(test_atr_feature test_atr_feature.cpp)

target_link_libraries(
  test_ema_feature
  GTest::gtest_main
  market
  backtest_market
  feature
  ema_feature
  vector_view
)

target_link_libraries(
  test_sma_feature
  GTest::gtest_main
  market
  backtest_market
  feature
  sma_feature
  vector_view
)

target_link_libraries(
  test_atr_feature
  GTest::gtest_main
  market
  backtest_market
  feature
  atr_feature
  vector_view
)

gtest_discover_tests(test_ema_feature)
gtest_discover_tests(test_sma_feature)
gtest_discover_tests(test_atr_feature)
