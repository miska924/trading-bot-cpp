#include <iostream>

#include "markets/backtest_market.h"


int main() {
    TradingBot::BacktestMarket market = TradingBot::BacktestMarket();
    std::cout << "Market time: " << market.time() << std::endl;
    return 0;
}
