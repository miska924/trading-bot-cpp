#include <iostream>

#include "markets/backtest_market.h"


int main() {
    trading_bot::BacktestMarket market = trading_bot::BacktestMarket();
    std::cout << "Market time: " << market.time() << std::endl;
    return 0;
}
