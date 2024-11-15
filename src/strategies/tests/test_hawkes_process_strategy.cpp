#include <gtest/gtest.h>

#include "markets/backtest_market.h"
#include "plotting/plotting.h"
#include "strategies/hawkes_process_strategy.h"
#include "traders/simple_trader.h"
#include "traders/sltp_trader.h"


using namespace TradingBot;


TEST(HawkesProcessStrategyTest, TestHawkesProcessStrategy) {
    std::string testDataFileName = "../../../../test_data/btcusdt_15m_3y.csv";
    std::vector<Candle> candles = readCSVFile(testDataFileName);

    BacktestMarket market(candles);
    HawkesProcessStrategy strategy;

    strategy.enableSavingPlots();
    DynamicPercentSLTrader(&strategy, &market, 0.05, 0.05).run();
    // SimpleTrader(&strategy, &market).run();
    plot(
        "TestHawkesProcessStrategy.png",
        market.getCandles().toVector(),
        market.getOrderHistory(),
        market.getBalanceHistory()
    );

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        439.40371925631598
    );
}


TEST(HawkesProcessStrategyTest, TestHawkesProcessRiskStrategy) {
    std::string testDataFileName = "../../../../test_data/btcusdt_15m_3y.csv";
    std::vector<Candle> candles = readCSVFile(testDataFileName);

    BacktestMarket market(candles);
    HawkesProcessStrategy strategy(
        10000, /* atrPeriod */ 
        10000, /* normRangePeriod */ 
        50 /* normalRangeSmoothPeriod */
    );

    strategy.enableSavingPlots();
    DynamicPercentSLTrader(&strategy, &market, 10, 1).run();
    plot(
        "TestHawkesProcessRiskStrategy.png",
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