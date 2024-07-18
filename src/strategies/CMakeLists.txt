add_library(strategy strategy.cpp)
add_library(autofit_strategy autofit_strategy.cpp)
add_library(macd_strategy macd_strategy.cpp)
add_library(dummy_strategy dummy_strategy.cpp)
add_library(averaging_strategy averaging_strategy.cpp)
add_library(hawks_process_strategy hawks_process_strategy.cpp)
add_library(sma_bounce_strategy sma_bounce_strategy.cpp)

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
    PRIVATE
    backtest_market
    fitter
    market
    Matplot++::matplot
    plotting
    plotting
    strategy
    vector_view
)

target_link_libraries(
    averaging_strategy
    atr_feature
    feature
    market
    strategy
    vector_view
)

target_link_libraries(
    hawks_process_strategy
    atr_feature
    feature
    market
    strategy
    vector_view
)

target_link_libraries(
    sma_bounce_strategy
    sma_feature
    atr_feature
    feature
    market
    strategy
    vector_view
)

add_subdirectory(tests)
