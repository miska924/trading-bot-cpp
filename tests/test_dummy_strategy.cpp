#include "gtest/gtest.h"
#include "strategies/dummy_strategy.h"
#include "markets/dummy_market.h"


TEST(DummyStrategy, TestRun) {
    trading_bot::DummyMarket market = trading_bot::DummyMarket();
    trading_bot::DummyStrategy strategy = trading_bot::DummyStrategy(&market);
    strategy.run();

    EXPECT_EQ(market.history().empty(), true);
}