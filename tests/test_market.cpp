#include "gtest/gtest.h"
#include "markets/market.h"


TEST(DummyMarket, TestInit) {
    trading_bot::DummyMarket market = trading_bot::DummyMarket();
    EXPECT_NE(market.time(), 0);
}
