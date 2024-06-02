# Исполняемый файл и его исходный код
add_executable(trading_bot_app main.cpp)

# Связываем исполняемый файл с библиотекой backtesting
target_link_libraries(
    trading_bot_app
    market
    backtest_market
    macd_strategy
    strategy
    fitter
    matplot
    plotting
    ema_feature
    feature
    vector_view
    tinkoff_market
)
