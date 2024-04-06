#include <matplot/matplot.h>
#include <gtest/gtest.h>
#include <gtest/internal/gtest-port.h>
#include <thread>

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
    EXPECT_EQ(market.getOrderHistory().size(), 54 * 2);
}

TEST(MACDStrategyTest, TestMACDStrategyLarge) {
    std::string testDataFileName = "../../test_data/btcusdt_15m_3y.csv";
    TradingBot::BacktestMarket market = TradingBot::BacktestMarket(testDataFileName);
    TradingBot::MACDStrategy strategy = TradingBot::MACDStrategy(&market, 50, 400);
    strategy.run();
    TradingBot::plot("TestMACDStrategyLarge.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());
    EXPECT_EQ(market.getOrderHistory().size(), 978 * 2);
}

TEST(MACD5CandlesStrategyTest, TestMACD5CandlesStrategyFit) {
    const std::string testDataFileName = "../../test_data/btcusdt_15m_3y.csv";
    const std::vector<TradingBot::Candle> candles = TradingBot::readCSVFile(testDataFileName);

    TradingBot::MACD5CandlesStrategyFitter fitter(candles, 1, 100);
    fitter.fit();
    fitter.plotBestStrategy();
}
