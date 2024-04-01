add_library(market market.cpp)
add_library(backtest_market backtest_market.cpp)

target_include_directories(market
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)
target_include_directories(backtest_market
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)
