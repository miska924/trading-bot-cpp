add_library(fitter fitter.cpp)

target_link_libraries(
  fitter
  PRIVATE
  backtest_market
  market
  Matplot++::matplot
  plotting
  strategy
  vector_view
  simple_trader
)

add_subdirectory(tests)
