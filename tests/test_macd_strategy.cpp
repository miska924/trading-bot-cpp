#include <matplot/matplot.h>
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
    TradingBot::plot("TestMACDStrategy.png", market.getCandles(), market.history());
    EXPECT_EQ(market.history().size(), 54);
}

TEST(MACDStrategyTest, TestMACDStrategyLarge) {
    std::string testDataFileName = "../../test_data/btcusdt_15m_3y.csv";
    TradingBot::BacktestMarket market = TradingBot::BacktestMarket(testDataFileName);
    TradingBot::MACDStrategy strategy = TradingBot::MACDStrategy(&market, 50, 400);
    strategy.run();
    TradingBot::plot("TestMACDStrategyLarge.png", market.getCandles(), market.history());
    EXPECT_EQ(market.history().size(), 978);
}

// TEST(MACDStrategyTestLarge, TestMACDStrategyLargest) {
//     std::string testDataFileName = "../../test_data/btcusdt_1m_3y.csv";
//     TradingBot::BacktestMarket market = TradingBot::BacktestMarket(testDataFileName);
//     TradingBot::MACDStrategy strategy = TradingBot::MACDStrategy(&market, 750, 6000);
//     strategy.run();
//     TradingBot::plot("TestMACDStrategyLargest.png", market.getCandles(), market.history());
//     EXPECT_EQ(market.history().size(), 3140);
// }
