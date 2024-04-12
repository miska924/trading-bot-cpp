#include <iostream>
#include <vector>

#include "markets/backtest_market.h"
#include "strategies/autofit_strategy.h"
#include "strategies/macd_strategy.h"


int main() {
    const std::string testDataFileName = "../../test_data/btcusdt_15m_3y.csv";
    std::vector<TradingBot::Candle> candles = TradingBot::readCSVFile(testDataFileName);
    TradingBot::BacktestMarket market(candles);

    TradingBot::AutoFitStrategy<TradingBot::MACDHoldSlowStrategy> strategy(&market, 10000, 1000, 100);

    strategy.run();
    TradingBot::plot("BestStrategy.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory());

    return 0;
}
