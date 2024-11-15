add_executable(test_fitter test_fitter.cpp)

target_link_libraries(
  test_fitter
  PRIVATE
  backtest_market
  market
  ema_crossover_strategy
  averaging_strategy
  atr_feature
  strategy
  ema_feature
  feature
  fitter
  plotting
  vector_view
  test_data
  Matplot++::matplot
  GTest::gtest_main
)

gtest_discover_tests(test_fitter)
