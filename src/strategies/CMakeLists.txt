add_library(strategy strategy.cpp)
add_library(autofit_strategy autofit_strategy.cpp)
add_library(macd_strategy macd_strategy.cpp)
add_library(dummy_strategy dummy_strategy.cpp)

target_link_libraries(
    strategy
    market
    vector_view
)

target_link_libraries(
    dummy_strategy
    market
    strategy
    vector_view
)

target_link_libraries(
    macd_strategy
    ema_feature
    feature
    market
    strategy
    vector_view
)

target_link_libraries(
    autofit_strategy
    backtest_market
    fitter
    market
    matplot
    plotting
    plotting
    strategy
    vector_view
)

add_subdirectory(tests)
