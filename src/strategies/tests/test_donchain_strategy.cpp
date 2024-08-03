#include <gtest/gtest.h>

#include "markets/backtest_market.h"
#include "plotting/plotting.h"
#include "strategies/donchain_strategy.h"


TEST(DonchainStrategyTest, TestDonchainStrategy) {
    std::string testDataFileName = "../../../../test_data/data.csv";
    std::vector<TradingBot::Candle> candles = TradingBot::readCSVFile(testDataFileName);

    TradingBot::BacktestMarket market = TradingBot::BacktestMarket(candles);
    int startTime = market.time();
    TradingBot::DonchainStrategy strategy = TradingBot::DonchainStrategy(&market);
    strategy.run();
    TradingBot::plot("TestDonchainStrategy.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory());
    EXPECT_EQ(market.getOrderHistory().size(), 112);
}

TEST(DonchainStrategyTest, TestDonchainStrategyLarge) {
    std::string testDataFileName = "../../../../test_data/btcusdt_15m_3y.csv";
    std::vector<TradingBot::Candle> candles = TradingBot::readCSVFile(testDataFileName);

    TradingBot::BacktestMarket market = TradingBot::BacktestMarket(candles);
    TradingBot::DonchainStrategy strategy = TradingBot::DonchainStrategy(&market, 1000);
    strategy.run();
    TradingBot::plot("TestDonchainStrategyLarge.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory());
    EXPECT_EQ(market.getOrderHistory().size(), 130);
}

TEST(DonchainStrategyTest, TestDonchainStrategyGAZP) {
    std::string testDataFileName = "../../../../test_data/gazp_1h_3y.csv";
    std::vector<TradingBot::Candle> candles = TradingBot::readCSVFile(testDataFileName);

    TradingBot::BacktestMarket market = TradingBot::BacktestMarket(candles, true, false, 0.003, {.assetA = 2000});
    TradingBot::DonchainStrategy strategy = TradingBot::DonchainStrategy(&market, 200);
    strategy.run();
    TradingBot::plot("TestDonchainStrategyGAZP.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory());
    EXPECT_EQ(market.getOrderHistory().size(), 102);
}

