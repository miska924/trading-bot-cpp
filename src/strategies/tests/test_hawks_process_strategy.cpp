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
        439.40371925631598
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
        {},
        true
    );

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        43128.258981971027
    );
}