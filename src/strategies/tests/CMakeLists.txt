add_executable(test_dummy_strategy test_dummy_strategy.cpp)
add_executable(test_macd_strategy test_macd_strategy.cpp)
add_executable(test_autofit_strategy test_autofit_strategy.cpp)
add_executable(test_averaging_strategy test_averaging_strategy.cpp)
add_executable(test_hawks_process_strategy test_hawks_process_strategy.cpp)

target_link_libraries(
  test_dummy_strategy
  GTest::gtest_main
  market
  dummy_market
  backtest_market
  strategy
  dummy_strategy
  vector_view
)

target_link_libraries(
  test_macd_strategy
  GTest::gtest_main
  market
  backtest_market
  strategy
  macd_strategy
  feature
  ema_feature
  fitter
  plotting
  vector_view
)

target_link_libraries(
  test_autofit_strategy
  PRIVATE
  GTest::gtest_main
  backtest_market
  market
  macd_strategy
  strategy
  ema_feature
  feature
  fitter
  plotting
  Matplot++::matplot
  autofit_strategy
  vector_view
  averaging_strategy
  atr_feature
  hawks_process_strategy
)

target_link_libraries(
  test_averaging_strategy
  GTest::gtest_main
  market
  backtest_market
  strategy
  averaging_strategy
  feature
  atr_feature
  fitter
  plotting
  vector_view
)

target_link_libraries(
  test_hawks_process_strategy
  GTest::gtest_main
  market
  backtest_market
  strategy
  hawks_process_strategy
  feature
  atr_feature
  fitter
  plotting
  vector_view
)


gtest_discover_tests(test_dummy_strategy)
gtest_discover_tests(test_macd_strategy)
gtest_discover_tests(test_autofit_strategy)
gtest_discover_tests(test_averaging_strategy)
gtest_discover_tests(test_hawks_process_strategy)
