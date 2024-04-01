#include "gtest/gtest.h"
#include "markets/market.h"


TEST(BacktestingTest, TestAddSimple) {
    trading_bot::DummyMarket market = trading_bot::DummyMarket();
    EXPECT_NE(market.time(), 0);
}
