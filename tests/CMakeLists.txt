include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(test_dummy_market test_dummy_market.cpp)
add_executable(test_backtest_market test_backtest_market.cpp)
add_executable(test_dummy_strategy test_dummy_strategy.cpp)
add_executable(test_ema_feature test_ema_feature.cpp)
add_executable(test_macd_strategy test_macd_strategy.cpp)
add_executable(test_autofit_strategy test_autofit_strategy.cpp)
add_executable(test_fitter test_fitter.cpp)

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
  test_dummy_strategy
  gtest_main
  market
  dummy_market
  backtest_market
  strategy
  dummy_strategy
  vector_view
)
target_link_libraries(
  test_ema_feature
  gtest_main
  market
  backtest_market
  feature
  ema_feature
  vector_view
)
target_link_libraries(
  test_macd_strategy
  gtest_main
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
)
target_link_libraries(
  test_autofit_strategy
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
  autofit_strategy
  vector_view
)

include(GoogleTest)

gtest_discover_tests(test_dummy_market)
gtest_discover_tests(test_backtest_market)
gtest_discover_tests(test_dummy_strategy)
gtest_discover_tests(test_ema_feature)
gtest_discover_tests(test_macd_strategy)
gtest_discover_tests(test_autofit_strategy)
gtest_discover_tests(test_fitter)
