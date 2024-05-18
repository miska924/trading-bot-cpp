#include <gtest/gtest.h>

#include "markets/backtest_market.h"
#include "plotting/plotting.h"
#include "strategies/hawks_process_strategy.h"


TEST(HawksProcessStrategyTest, TestHawksProcessStrategy) {
    std::string testDataFileName = "../../../../test_data/btcusdt_15m_3y.csv";
    std::vector<TradingBot::Candle> candles = TradingBot::readCSVFile(testDataFileName);

    TradingBot::BacktestMarket market(candles);
    TradingBot::HawksProcessStrategy strategy(&market);

    strategy.run();

    TradingBot::plot(
        "TestHawksProcessStrategy.png",
        market.getCandles().toVector(),
        market.getOrderHistory(),
        market.getBalanceHistory()
    );

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        442.74545519997986
    );
}


TEST(HawksProcessStrategyTest, TestHawksProcessRiskStrategy) {
    std::string testDataFileName = "../../../../test_data/btcusdt_15m_3y.csv";
    std::vector<TradingBot::Candle> candles = TradingBot::readCSVFile(testDataFileName);

    TradingBot::BacktestMarket market(candles);
    TradingBot::HawksProcessStrategy strategy(
        &market,
        10000, /* atrPeriod */ 
        10000, /* normRangePeriod */ 
        50, /* normalRangeSmoothPeriod */ 
        10, /* risk */ 
        0, /* preventDrawdown */ 
        0.0 /* preventDrawdownCoeff */
    );

    strategy.run();

    TradingBot::plot(
        "TestHawksProcessRiskStrategy.png",
        market.getCandles().toVector(),
        market.getOrderHistory(),
        market.getBalanceHistory(),
        true
    );

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        46089.509270620249
    );
}

TEST(HawksProcessStrategyTest, TestHawksProcess1hStrategy) {
    std::string testDataFileName = "../../../../test_data/btcusdt_1h_5y.csv";
    std::vector<TradingBot::Candle> candles = TradingBot::readCSVFile(testDataFileName);

    TradingBot::BacktestMarket market(candles);
    TradingBot::HawksProcessStrategy strategy(
        &market,
        366, /* atrPeriod */ 
        366, /* normRangePeriod */ 
        5, /* normalRangeSmoothPeriod */ 
        1, /* risk */ 
        1, /* preventDrawdown */ 
        0.01 /* preventDrawdownCoeff */
    );

    strategy.run();

    TradingBot::plot(
        "TestHawksProcess1hStrategy.png",
        market.getCandles().toVector(),
        market.getOrderHistory(),
        market.getBalanceHistory(),
        false
    );

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        46089.509270620249
    );
}