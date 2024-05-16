#include <gtest/gtest.h>

#include "markets/backtest_market.h"
#include "plotting/plotting.h"
#include "strategies/averaging_strategy.h"


TEST(AveragingStrategyTest, TestAveragingStrategy) {
    std::string testDataFileName = "../../../../test_data/data.csv";
    std::vector<TradingBot::Candle> candles = TradingBot::readCSVFile(testDataFileName);
    TradingBot::BacktestMarket market = TradingBot::BacktestMarket(candles);
    int startTime = market.time();
    TradingBot::AveragingStrategy strategy(&market);
    strategy.run();
    TradingBot::plot("TestAveragingStrategy.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory());
    EXPECT_EQ(market.getOrderHistory().size(), 1);
}

TEST(AveragingStrategyTest, TestAveragingStrategyLarge) {
    std::string testDataFileName = "../../../../test_data/btcusdt_15m_3y.csv";
    std::vector<TradingBot::Candle> candles = TradingBot::readCSVFile(testDataFileName);
    TradingBot::BacktestMarket market = TradingBot::BacktestMarket(candles);
    TradingBot::AveragingStrategy strategy = TradingBot::AveragingStrategy(&market);
    strategy.run();
    TradingBot::plot("TestAveragingStrategyLarge.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory());
    EXPECT_EQ(market.getOrderHistory().size(), 53);
}
