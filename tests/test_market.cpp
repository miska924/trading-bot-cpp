#include "gtest/gtest.h"
#include "markets/market.h"


TEST(BacktestingTest, TestAddSimple) {
    trading_bot::Market market = trading_bot::Market();
    EXPECT_NE(market.time(), 0);
}
