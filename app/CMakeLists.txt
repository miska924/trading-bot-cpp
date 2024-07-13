add_executable(trading_bot_app main.cpp)

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
    hawks_process_strategy
    averaging_strategy
    dummy_strategy
    date_time
)
