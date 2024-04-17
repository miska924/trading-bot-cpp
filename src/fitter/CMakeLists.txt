add_library(fitter fitter.cpp)

target_link_libraries(
  fitter
  backtest_market
  market
  matplot
  plotting
  strategy
  vector_view
)

add_subdirectory(tests)
