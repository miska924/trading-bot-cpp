#include <iostream>
#include <vector>

#include "markets/backtest_market.h"
#include "strategies/autofit_strategy.h"
#include "strategies/macd_strategy.h"


int main() {
    const std::string testDataFileName = "../../test_data/btcusdt_15m_3y.csv";
    TradingBot::BacktestMarket market(testDataFileName);

    TradingBot::AutoFitStrategy<TradingBot::MACDHoldSlowStrategy> strategy(&market, 10000, 1000, 100);

    strategy.run();
    TradingBot::plot("BestStrategy.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());

    return 0;
}
