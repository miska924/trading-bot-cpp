add_executable(test_dummy_strategy test_dummy_strategy.cpp)
add_executable(test_ema_crossover_strategy test_ema_crossover_strategy.cpp)
add_executable(test_autofit_strategy test_autofit_strategy.cpp)
add_executable(test_averaging_strategy test_averaging_strategy.cpp)
add_executable(test_hawkes_process_strategy test_hawkes_process_strategy.cpp)
add_executable(test_donchain_strategy test_donchain_strategy.cpp)
add_executable(test_ordinal_patterns_strategy test_ordinal_patterns_strategy.cpp)

target_link_libraries(
  test_dummy_strategy
  PRIVATE
  GTest::gtest_main
  market
  dummy_market
  backtest_market
  strategy
  dummy_strategy
  vector_view
  simple_trader
)

target_link_libraries(
  test_ema_crossover_strategy
  PRIVATE
  GTest::gtest_main
  market
  backtest_market
  strategy
  ema_crossover_strategy
  feature
  ema_feature
  fitter
  plotting
  vector_view
  simple_trader
)

target_link_libraries(
  test_autofit_strategy
  PRIVATE
  GTest::gtest_main
  backtest_market
  market
  ema_crossover_strategy
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
  hawkes_process_strategy
  sma_bounce_strategy
  sma_feature
  donchain_strategy
  simple_trader
)

target_link_libraries(
  test_averaging_strategy
  PRIVATE
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
  simple_trader
)

target_link_libraries(
  test_hawkes_process_strategy
  PRIVATE
  GTest::gtest_main
  market
  backtest_market
  strategy
  hawkes_process_strategy
  feature
  atr_feature
  fitter
  plotting
  vector_view
  simple_trader
)

target_link_libraries(
  test_donchain_strategy
  GTest::gtest_main
  market
  backtest_market
  strategy
  donchain_strategy
  function_queue
  fitter
  plotting
  vector_view
  simple_trader
)

target_link_libraries(
  test_ordinal_patterns_strategy
  GTest::gtest_main
  market
  backtest_market
  strategy
  ordinal_patterns_strategy
  function_queue
  fitter
  plotting
  vector_view
  simple_trader
)

gtest_discover_tests(test_dummy_strategy)
gtest_discover_tests(test_ema_crossover_strategy)
gtest_discover_tests(test_autofit_strategy)
gtest_discover_tests(test_averaging_strategy)
gtest_discover_tests(test_hawkes_process_strategy)
gtest_discover_tests(test_donchain_strategy)
gtest_discover_tests(test_ordinal_patterns_strategy)
