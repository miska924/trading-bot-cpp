#include <gtest/gtest.h>

#include "markets/backtest_market.h"
#include "plotting/plotting.h"
#include "strategies/averaging_strategy.h"
#include "traders/simple_trader.h"


using namespace TradingBot;


std::vector<Candle> btcusdt15m10dCandles = readCSVFile("../../../../test_data/btcusdt_15m_10d.csv");
std::vector<Candle> btcusdt15m3yCandles = readCSVFile("../../../../test_data/btcusdt_15m_3y.csv");
std::vector<Candle> gazp1h3yCandles = readCSVFile("../../../../test_data/gazp_1h_3y.csv");


TEST(AveragingStrategyTest, TestAveragingStrategy) {
    BacktestMarket market(btcusdt15m10dCandles);
    AveragingStrategy strategy;

    strategy.enableSavingPlots();
    SimpleTrader(&strategy, &market).run();
    plot("TestAveragingStrategy.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory(), strategy.getPlots());
    EXPECT_EQ(market.getOrderHistory().size(), 1);
    EXPECT_EQ(market.getBalance().asAssetA(), 100.1248418723286);
}

TEST(AveragingStrategyTest, TestAveragingStrategyLarge) {
    BacktestMarket market(btcusdt15m3yCandles);
    AveragingStrategy strategy = AveragingStrategy();

    strategy.enableSavingPlots();
    SimpleTrader(&strategy, &market).run();
    plot("TestAveragingStrategyLarge.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory(), strategy.getPlots());
    EXPECT_EQ(market.getOrderHistory().size(), 53);
    EXPECT_EQ(market.getBalance().asAssetA(), 46.713651940826196);
}

TEST(AveragingStrategyTest, TestAveragingStrategyGAZP) {
    BacktestMarket market(gazp1h3yCandles);
    AveragingStrategy strategy = AveragingStrategy();

    strategy.enableSavingPlots();
    SimpleTrader(&strategy, &market).run();
    plot("TestAveragingStrategyGAZP.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory(), strategy.getPlots());
    
    EXPECT_EQ(market.getOrderHistory().size(), 17);
    EXPECT_EQ(market.getBalance().asAssetA(), 155.51099292552749);
}
