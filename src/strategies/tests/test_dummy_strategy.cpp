#include <gtest/gtest.h>

#include "markets/dummy_market.h"
#include "strategies/dummy_strategy.h"


TEST(DummyStrategyTest, TestRunStrategy) {
    TradingBot::DummyMarket market = TradingBot::DummyMarket();
    TradingBot::DummyStrategy strategy = TradingBot::DummyStrategy(&market);
    strategy.run();

    EXPECT_EQ(market.getOrderHistory().empty(), false);
}
