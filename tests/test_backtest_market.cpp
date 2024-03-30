#include "gtest/gtest.h"
#include "markets/backtest_market.h"

#include <fstream>


TEST(BacktestingTest, TestStartTime) {
    trading_bot::BacktestMarket market = trading_bot::BacktestMarket();
    EXPECT_EQ(market.time(), 0);
}


TEST(BacktestingTest, TestHistory) {
    trading_bot::BacktestMarket market = trading_bot::BacktestMarket();

    std::vector<trading_bot::Order> orders = {
        {
            .side = trading_bot::OrderSide::BUY,
            .amount = 1
        },
        {
            .side = trading_bot::OrderSide::SELL,
            .amount = 10.3
        },
    };

    EXPECT_EQ(market.history().size(), 0);

    for (const auto& order : orders) {
        market.order(order);
    }
    
    EXPECT_EQ(market.history().size(), 2);
    EXPECT_EQ(market.history()[0], orders[0]);
    EXPECT_EQ(market.history()[1], orders[1]);
}


TEST(BacktestingTest, TestRandomCandlesGeneration) {
    trading_bot::BacktestMarket market = trading_bot::BacktestMarket();
    EXPECT_EQ(market.allCandles(), nullptr);
    market.finish();
    EXPECT_EQ(
        market.allCandles()->size(),
        trading_bot::DEFAULT_RANDOM_BACKTEST_MARKET_SIZE
    );
}

TEST(BacktestingTest, TestCandlesFromFile) {
    std::string testDataFileName = "../../test_data/data.csv";
    std::ifstream file(testDataFileName);
    
    size_t linesCount = 0;
    std::string line;
    std::getline(file, line); // skip header
    while (std::getline(file, line)) {
        linesCount++;
    }

    trading_bot::BacktestMarket market = trading_bot::BacktestMarket("../../test_data/data.csv");
    EXPECT_EQ(market.allCandles(), nullptr);
    market.finish();
    EXPECT_NE(market.allCandles(), nullptr);
    EXPECT_EQ(
        market.allCandles()->size(),
        linesCount
    );
}
