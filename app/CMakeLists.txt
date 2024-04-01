# Исполняемый файл и его исходный код
add_executable(trading_bot_app main.cpp)

# Связываем исполняемый файл с библиотекой backtesting
target_link_libraries(trading_bot_app PRIVATE market)
target_link_libraries(trading_bot_app PRIVATE backtest_market)