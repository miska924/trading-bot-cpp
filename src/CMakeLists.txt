add_library(market market.cpp)
add_library(dummy_market dummy_market.cpp)
add_library(backtest_market backtest_market.cpp)
add_library(strategy strategy.cpp)
add_library(dummy_strategy dummy_strategy.cpp)

target_include_directories(market
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include/markets
)
target_include_directories(strategy
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include/strategies
)
target_include_directories(dummy_strategy
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include/strategies
)
target_include_directories(backtest_market
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include/markets
)
target_include_directories(dummy_market
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include/markets
)
