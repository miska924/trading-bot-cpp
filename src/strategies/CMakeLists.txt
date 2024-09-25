add_library(strategy strategy.cpp)
add_library(autofit_strategy autofit_strategy.cpp)
add_library(ema_crossover_strategy ema_crossover_strategy.cpp)
add_library(dummy_strategy dummy_strategy.cpp)
add_library(averaging_strategy averaging_strategy.cpp)
add_library(hawkes_process_strategy hawkes_process_strategy.cpp)
add_library(sma_bounce_strategy sma_bounce_strategy.cpp)
add_library(donchain_strategy donchain_strategy.cpp)
add_library(ordinal_patterns_strategy ordinal_patterns_strategy.cpp)

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
    ema_crossover_strategy
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
    simple_trader
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
    hawkes_process_strategy
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

target_link_libraries(
    donchain_strategy
    function_queue
    sma_feature
    atr_feature
    feature
    market
    strategy
    vector_view
)

target_link_libraries(
    ordinal_patterns_strategy
    feature
    market
    strategy
    vector_view
)

add_subdirectory(tests)
