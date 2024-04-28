add_library(market market.cpp)
add_library(dummy_market dummy_market.cpp)
add_library(backtest_market backtest_market.cpp)
add_library(tinkoff_market tinkoff_market.cpp)

target_link_libraries(
  market
  vector_view
)

target_link_libraries(
  dummy_market
  market
  vector_view
)

target_link_libraries(
  backtest_market
  market
  vector_view
)

target_link_libraries(
  tinkoff_market
  market
  vector_view
  tinkoff_api
  grpc++
  tinkoff_contracts
)

add_subdirectory(tinkoff)
add_subdirectory(tests)
