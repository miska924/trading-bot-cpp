#include <gtest/gtest.h>

#include "markets/backtest_market.h"
#include "plotting/plotting.h"
#include "strategies/donchain_strategy.h"
#include "traders/simple_trader.h"


using namespace TradingBot;

TEST(DonchainStrategyTest, TestDonchainStrategy) {
    std::string testDataFileName = "../../../../test_data/btcusdt_15m_10d.csv";
    std::vector<Candle> candles = readCSVFile(testDataFileName);

    BacktestMarket market = BacktestMarket(candles);
    DonchainStrategy strategy;
    
    SimpleTrader(&strategy, &market).run();

    plot("TestDonchainStrategy.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory());
    EXPECT_EQ(market.getOrderHistory().size(), 112);
}

TEST(DonchainStrategyTest, TestDonchainStrategyLarge) {
    std::string testDataFileName = "../../../../test_data/btcusdt_15m_3y.csv";
    std::vector<Candle> candles = readCSVFile(testDataFileName);

    BacktestMarket market = BacktestMarket(candles);
    DonchainStrategy strategy = DonchainStrategy(1000);
    
    SimpleTrader(&strategy, &market).run();

    plot("TestDonchainStrategyLarge.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory());
    EXPECT_EQ(market.getOrderHistory().size(), 130);
}

TEST(DonchainStrategyTest, TestDonchainStrategyGAZP) {
    std::string testDataFileName = "../../../../test_data/gazp_1h_3y.csv";
    std::vector<Candle> candles = readCSVFile(testDataFileName);

    BacktestMarket market = BacktestMarket(candles, true, false, 0.003, {.assetA = 2000});
    DonchainStrategy strategy(200);
    
    SimpleTrader(&strategy, &market).run();

    plot("TestDonchainStrategyGAZP.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory());
    EXPECT_EQ(market.getOrderHistory().size(), 102);
}

