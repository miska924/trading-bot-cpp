#include <gtest/gtest.h>

#include "markets/backtest_market.h"
#include "plotting/plotting.h"
#include "strategies/macd_strategy.h"
#include "traders/simple_trader.h"


using namespace TradingBot;


TEST(MACDStrategyTest, TestMACDStrategy) {
    std::string testDataFileName = "../../../../test_data/btcusdt_15m_10d.csv";
    std::vector<Candle> candles = readCSVFile(testDataFileName);

    BacktestMarket market(candles);
    MACDStrategy strategy;

    SimpleTrader(&strategy, &market).run();
    plot("TestMACDStrategy.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory());

    EXPECT_EQ(market.getOrderHistory().size(), 76);
}

TEST(MACDStrategyTest, TestMACDStrategyLarge) {
    std::string testDataFileName = "../../../../test_data/btcusdt_15m_3y.csv";
    std::vector<Candle> candles = readCSVFile(testDataFileName);

    BacktestMarket market(candles);
    MACDStrategy strategy(20, 40);

    SimpleTrader(&strategy, &market).run();
    plot("TestMACDStrategyLarge.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory());

    EXPECT_EQ(market.getOrderHistory().size(), 6908);
}

// TEST(MACDStrategyTest, TestMACDStrategyGAZP) {
//     std::string testDataFileName = "../../../../test_data/gazp_1h_2y.csv";
//     std::vector<Candle> candles = readCSVFile(testDataFileName);
//     BacktestMarket market = BacktestMarket(candles, true, false, 0.003, {.assetA = 2000});
//     MACDStrategy strategy = MACDStrategy(&market, 20, 40);
//     strategy.run();
//     plot("TestMACDStrategyGAZP.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory());
//     EXPECT_EQ(market.getOrderHistory().size(), 6908);
// }

