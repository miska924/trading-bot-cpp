#include <gtest/gtest.h>

#include "markets/dummy_market.h"


TEST(DummyMarketTest, TestInit) {
    TradingBot::DummyMarket market = TradingBot::DummyMarket();
    EXPECT_NE(market.time(), 0);
}
