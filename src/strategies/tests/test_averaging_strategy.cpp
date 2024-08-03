#include <gtest/gtest.h>

#include "markets/backtest_market.h"
#include "plotting/plotting.h"
#include "strategies/averaging_strategy.h"
#include "traders/simple_trader.h"


using namespace TradingBot;

TEST(AveragingStrategyTest, TestAveragingStrategy) {
    std::string testDataFileName = "../../../../test_data/btcusdt_15m_10d.csv";
    std::vector<Candle> candles = readCSVFile(testDataFileName);
    BacktestMarket market(candles);
    int startTime = market.time();
    AveragingStrategy strategy;
    SimpleTrader(&strategy, &market).run();
    plot("TestAveragingStrategy.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory());
    EXPECT_EQ(market.getOrderHistory().size(), 1);
}

TEST(AveragingStrategyTest, TestAveragingStrategyLarge) {
    std::string testDataFileName = "../../../../test_data/btcusdt_15m_3y.csv";
    std::vector<Candle> candles = readCSVFile(testDataFileName);
    BacktestMarket market = BacktestMarket(candles);
    AveragingStrategy strategy = AveragingStrategy();
    SimpleTrader(&strategy, &market).run();
    plot("TestAveragingStrategyLarge.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory());
    EXPECT_EQ(market.getOrderHistory().size(), 53);
}
