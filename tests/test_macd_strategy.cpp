#include <gtest/gtest.h>

#include "strategies/macd_strategy.h"
#include "markets/backtest_market.h"
#include "plotting/plotting.h"


TEST(MACDStrategyTest, TestMACDStrategy) {
    std::string testDataFileName = "../../test_data/data.csv";
    TradingBot::BacktestMarket market = TradingBot::BacktestMarket(testDataFileName);
    int startTime = market.time();
    TradingBot::MACDStrategy strategy = TradingBot::MACDStrategy(&market);
    strategy.run();
    TradingBot::plot("TestMACDStrategy.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());
    EXPECT_EQ(market.getOrderHistory().size(), 108);
}

TEST(MACDStrategyTest, TestMACDStrategyLarge) {
    std::string testDataFileName = "../../test_data/btcusdt_15m_3y.csv";
    TradingBot::BacktestMarket market = TradingBot::BacktestMarket(testDataFileName);
    TradingBot::MACDStrategy strategy = TradingBot::MACDStrategy(&market, 20, 40);
    strategy.run();
    TradingBot::plot("TestMACDStrategyLarge.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());
    EXPECT_EQ(market.getOrderHistory().size(), 12844);
}
