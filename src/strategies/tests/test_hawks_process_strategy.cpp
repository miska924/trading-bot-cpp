#include <gtest/gtest.h>

#include "markets/backtest_market.h"
#include "plotting/plotting.h"
#include "strategies/hawks_process_strategy.h"
#include "traders/simple_trader.h"


using namespace TradingBot;


TEST(HawksProcessStrategyTest, TestHawksProcessStrategy) {
    std::string testDataFileName = "../../../../test_data/btcusdt_15m_3y.csv";
    std::vector<Candle> candles = readCSVFile(testDataFileName);

    BacktestMarket market(candles);
    HawksProcessStrategy strategy;

    SimpleTrader(&strategy, &market).run();

    plot(
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
    std::vector<Candle> candles = readCSVFile(testDataFileName);

    BacktestMarket market(candles);
    HawksProcessStrategy strategy(
        10000, /* atrPeriod */ 
        10000, /* normRangePeriod */ 
        50, /* normalRangeSmoothPeriod */ 
        10, /* risk */ 
        0, /* preventDrawdown */ 
        0.0 /* preventDrawdownCoeff */
    );

    SimpleTrader(&strategy, &market).run();

    plot(
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