#include <gtest/gtest.h>

#include "markets/backtest_market.h"
#include "plotting/plotting.h"
#include "strategies/macd_strategy.h"
#include "traders/simple_trader.h"


using namespace TradingBot;


std::vector<Candle> btcusdt15m10dCandles = readCSVFile("../../../../test_data/btcusdt_15m_10d.csv");
std::vector<Candle> btcusdt15m3yCandles = readCSVFile("../../../../test_data/btcusdt_15m_3y.csv");
std::vector<Candle> gazp1h3yCandles = readCSVFile("../../../../test_data/gazp_1h_3y.csv");


TEST(MACDStrategyTest, TestMACDStrategy) {
    BacktestMarket market(btcusdt15m10dCandles);
    MACDStrategy strategy;

    strategy.enableSavingPlots();
    SimpleTrader(&strategy, &market).run();
    plot("TestMACDStrategy.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory(), strategy.getPlots());

    EXPECT_EQ(market.getOrderHistory().size(), 56);
    EXPECT_EQ(market.getBalance().asAssetA(), 86.922363544836927);
}

TEST(MACDStrategyTest, TestMACDStrategyLarge) {
    BacktestMarket market(btcusdt15m3yCandles);
    MACDStrategy strategy(20, 40);

    strategy.enableSavingPlots();
    SimpleTrader(&strategy, &market).run();
    plot("TestMACDStrategyLarge.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory(), strategy.getPlots());

    EXPECT_EQ(market.getOrderHistory().size(), 4824);
    EXPECT_EQ(market.getBalance().asAssetA(), 0.01000386245488198);
}

TEST(MACDStrategyTest, TestMACDStrategyGAZP) {
    BacktestMarket market(gazp1h3yCandles, true, false, 0.003, {.assetA = 2000});
    MACDStrategy strategy(64, 124);

    strategy.enableSavingPlots();
    SimpleTrader(&strategy, &market).run();
    plot("TestMACDStrategyGAZP.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory(), strategy.getPlots());

    EXPECT_EQ(market.getOrderHistory().size(), 224);
    EXPECT_EQ(market.getBalance().asAssetA(), 4523.7747167896732);
}
