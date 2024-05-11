#include <gtest/gtest.h>

#include "markets/tinkoff_market.h"


TEST(TinkoffMarketTest, TestInit) {
    TradingBot::TinkoffMarket market;
    market.order({
        .side=TradingBot::OrderSide::BUY
    });
    TradingBot::Balance balance = market.getBalance();
    EXPECT_EQ(balance.assetA, TradingBot::Balance().assetA);
    EXPECT_EQ(balance.assetB, TradingBot::Balance().assetB);
}
