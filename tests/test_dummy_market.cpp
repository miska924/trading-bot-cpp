#include <gtest/gtest.h>

#include "markets/dummy_market.h"


TEST(DummyMarket, TestInit) {
    TradingBot::DummyMarket market = TradingBot::DummyMarket();
    EXPECT_NE(market.time(), 0);
}
