#include <gtest/gtest.h>

#include "markets/dummy_market.h"
#include "strategies/dummy_strategy.h"
#include "traders/simple_trader.h"


using namespace TradingBot;

TEST(DummyStrategyTest, TestRunStrategy) {
    DummyMarket market;
    DummyStrategy strategy;

    SimpleTrader(&strategy, &market).run();

    EXPECT_EQ(market.getOrderHistory().empty(), false);
}
