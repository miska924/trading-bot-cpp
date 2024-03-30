#include "gtest/gtest.h"
#include "markets/backtest_market.h"


TEST(BacktestingTest, TestAddSimple) {
    trading_bot::BacktestMarket market = trading_bot::BacktestMarket();
    EXPECT_EQ(market.time(), 0);
}
