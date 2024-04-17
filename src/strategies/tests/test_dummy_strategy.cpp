#include <gtest/gtest.h>

#include "strategies/dummy_strategy.h"
#include "markets/dummy_market.h"


TEST(DummyStrategyTest, TestRun) {
    TradingBot::DummyMarket market = TradingBot::DummyMarket();
    TradingBot::DummyStrategy strategy = TradingBot::DummyStrategy(&market);
    strategy.run();

    EXPECT_EQ(market.getOrderHistory().empty(), false);
}