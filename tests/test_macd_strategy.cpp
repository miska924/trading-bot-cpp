#include "gtest/gtest.h"
#include "strategies/macd_strategy.h"
#include "markets/backtest_market.h"


TEST(MACDStrategy, TestMACDStrategy) {
    std::string testDataFileName = "../../test_data/data.csv";
    TradingBot::BacktestMarket market = TradingBot::BacktestMarket(testDataFileName);
    int startTime = market.time();
    TradingBot::MACDStrategy strategy = TradingBot::MACDStrategy(&market);
    strategy.run();
    EXPECT_EQ(market.history().size(), 54);
}

TEST(MACDStrategy, TestMACDStrategyLarge) {
    std::string testDataFileName = "../../test_data/btcusdt_15m_3y.csv";
    TradingBot::BacktestMarket market = TradingBot::BacktestMarket(testDataFileName);
    TradingBot::MACDStrategy strategy = TradingBot::MACDStrategy(&market);
    strategy.run();
    EXPECT_EQ(market.history().size(), 7458);
}
