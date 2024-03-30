#include "gtest/gtest.h"
#include "trading_bot/backtesting.h"


TEST(BacktestingTest, TestAddSimple) {
    EXPECT_EQ(trading_bot::add(1, 2), 3);
}

TEST(BacktestingTest, TestAddComplex) {
    EXPECT_EQ(trading_bot::add(1, 2), 3);
    EXPECT_EQ(trading_bot::add(1, 5), 6);
    EXPECT_EQ(trading_bot::add(3, 2), 5);
    EXPECT_EQ(trading_bot::add(9, 2), 11);
    EXPECT_EQ(trading_bot::add(7, 7), 14);
}
