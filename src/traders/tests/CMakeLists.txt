add_executable(test_sltp_trader test_sltp_trader.cpp)
target_link_libraries(
  test_sltp_trader
  GTest::gtest_main
  market
  backtest_market
  ema_crossover_strategy
  strategy
  plotting
  vector_view
  sltp_trader
  autofit_strategy
  simple_trader
)

gtest_discover_tests(test_sltp_trader)
