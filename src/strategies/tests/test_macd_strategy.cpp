#include <gtest/gtest.h>

#include "markets/backtest_market.h"
#include "plotting/plotting.h"
#include "strategies/macd_strategy.h"


TEST(MACDStrategyTest, TestMACDStrategy) {
    std::string testDataFileName = "../../../../test_data/data.csv";
    std::vector<TradingBot::Candle> candles = TradingBot::readCSVFile(testDataFileName);
    TradingBot::BacktestMarket market = TradingBot::BacktestMarket(candles);
    int startTime = market.time();
    TradingBot::MACDStrategy strategy = TradingBot::MACDStrategy(&market);
    strategy.run();
    TradingBot::plot("TestMACDStrategy.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory());
    EXPECT_EQ(market.getOrderHistory().size(), 108);
}

TEST(MACDStrategyTest, TestMACDStrategyLarge) {
    std::string testDataFileName = "../../../../test_data/btcusdt_15m_3y.csv";
    std::vector<TradingBot::Candle> candles = TradingBot::readCSVFile(testDataFileName);
    TradingBot::BacktestMarket market = TradingBot::BacktestMarket(candles);
    TradingBot::MACDStrategy strategy = TradingBot::MACDStrategy(&market, 20, 40);
    strategy.run();
    TradingBot::plot("TestMACDStrategyLarge.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory());
    EXPECT_EQ(market.getOrderHistory().size(), 12844);
}
