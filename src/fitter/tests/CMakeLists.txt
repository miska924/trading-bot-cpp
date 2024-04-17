add_executable(test_fitter test_fitter.cpp)

target_link_libraries(
  test_fitter
  gtest_main
  backtest_market
  market
  macd_strategy
  strategy
  ema_feature
  feature
  fitter
  plotting
  matplot
  vector_view
  test_data
)

gtest_discover_tests(test_fitter)
