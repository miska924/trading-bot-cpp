#include <gtest/gtest.h>

#include "markets/backtest_market.h"
#include "plotting/plotting.h"
#include "strategies/donchain_strategy.h"
#include "traders/simple_trader.h"


using namespace TradingBot;


std::vector<Candle> btcusdt15m10dCandles = readCSVFile("../../../../test_data/btcusdt_15m_10d.csv");
std::vector<Candle> btcusdt15m3yCandles = readCSVFile("../../../../test_data/btcusdt_15m_3y.csv");
std::vector<Candle> gazp1h3yCandles = readCSVFile("../../../../test_data/gazp_1h_3y.csv");


TEST(DonchainStrategyTest, TestDonchainStrategy) {
    BacktestMarket market(btcusdt15m10dCandles);
    DonchainStrategy strategy;
    
    strategy.enableSavingPlots();
    SimpleTrader(&strategy, &market).run();
    plot("TestDonchainStrategy.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory(), strategy.getPlots());
    EXPECT_EQ(market.getOrderHistory().size(), 112);
    EXPECT_EQ(market.getBalance().asAssetA(), 85.157629126381394);
}

TEST(DonchainStrategyTest, TestDonchainStrategyLarge) {
    BacktestMarket market(btcusdt15m3yCandles);
    DonchainStrategy strategy(1000);
    
    strategy.enableSavingPlots();
    SimpleTrader(&strategy, &market).run();
    plot("TestDonchainStrategyLarge.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory(), strategy.getPlots());
    EXPECT_EQ(market.getOrderHistory().size(), 130);
    EXPECT_EQ(market.getBalance().asAssetA(), 155.27736360752095);
}

TEST(DonchainStrategyTest, TestDonchainLastWinnerStrategyLarge) {
    BacktestMarket market(btcusdt15m3yCandles);
    DonchainLastWinnerStrategy strategy(1000);
    
    strategy.enableSavingPlots();
    SimpleTrader(&strategy, &market).run();
    plot("TestDonchainLastWinnerStrategyLarge.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory(), strategy.getPlots());
    EXPECT_EQ(market.getOrderHistory().size(), 87);
    EXPECT_EQ(market.getBalance().asAssetA(), 358.69424670531168);
}

TEST(DonchainStrategyTest, TestDonchainLastLoserStrategyLarge) {
    BacktestMarket market(btcusdt15m3yCandles);
    DonchainLastLoserStrategy strategy(1000);
    
    strategy.enableSavingPlots();
    SimpleTrader(&strategy, &market).run();
    plot("TestDonchainLastLoserStrategyLarge.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory(), strategy.getPlots());
    EXPECT_EQ(market.getOrderHistory().size(), 42);
    EXPECT_EQ(market.getBalance().asAssetA(), 43.289616444584333);
}

TEST(DonchainStrategyTest, TestDonchainStrategyGAZP) {
    BacktestMarket market(gazp1h3yCandles, true, false, 0.003, {.assetA = 2000});
    DonchainStrategy strategy(200);
    
    strategy.enableSavingPlots();
    SimpleTrader(&strategy, &market).run();
    plot("TestDonchainStrategyGAZP.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory(), strategy.getPlots());
    EXPECT_EQ(market.getOrderHistory().size(), 102);
    EXPECT_EQ(market.getBalance().asAssetA(), 4106.9520661293836);
}

TEST(DonchainStrategyTest, TestDonchainLastLoserStrategyGAZP) {
    BacktestMarket market(gazp1h3yCandles, true, false, 0.003, {.assetA = 2000});
    DonchainLastLoserStrategy strategy(200);
    
    strategy.enableSavingPlots();
    SimpleTrader(&strategy, &market).run();
    plot("TestDonchainLastLoserStrategyGAZP.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory(), strategy.getPlots());
    EXPECT_EQ(market.getOrderHistory().size(), 37);
    EXPECT_EQ(market.getBalance().asAssetA(), 4669.922505452193);

}

TEST(DonchainStrategyTest, TestDonchainLastWinnerStrategyGAZP) {
    BacktestMarket market(gazp1h3yCandles, true, false, 0.003, {.assetA = 2000});
    DonchainLastWinnerStrategy strategy(200);
    
    strategy.enableSavingPlots();
    SimpleTrader(&strategy, &market).run();
    plot("TestDonchainLastWinnerStrategyGAZP.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory(), strategy.getPlots());
    EXPECT_EQ(market.getOrderHistory().size(), 64);
    EXPECT_EQ(market.getBalance().asAssetA(), 1758.8951685320117);
}
