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
    EXPECT_EQ(market.getCandles().size(), 1);
    market.finish();
    EXPECT_EQ(
        market.getCandles().size(),
        trading_bot::DEFAULT_RANDOM_BACKTEST_MARKET_SIZE
    );
}


TEST(BacktestingTest, TestReadCSVCandle) {
    std::string csvCandle = "2023-11-08 06:00:00,1699412400.0,35306.61,35321.37,35260.0,35288.65,182.86608";
    trading_bot::Candle candle = trading_bot::readCSVCandle(csvCandle);
    EXPECT_EQ(candle, trading_bot::Candle({
        .time = 1699412400,
        .open = 35306.61,
        .high = 35321.37,
        .low = 35260.0,
        .close = 35288.65,
        .volume = 182.86608
    }));
}


TEST(BacktestingTest, TestCandlesFromFile) {
    std::string testDataFileName = "../../test_data/data.csv";
    std::ifstream file(testDataFileName);
    
    size_t linesCount = 0;
    time_t start_time = 0;
    time_t finish_time = 0;
    std::string line;
    std::getline(file, line); // skip header
    bool first = true;
    while (std::getline(file, line)) {
        linesCount++;
        finish_time = trading_bot::readCSVCandle(line).time;
        if (first) {
            first = false;
            start_time = finish_time;
        }
    }
    file.close();

    trading_bot::BacktestMarket market = trading_bot::BacktestMarket(
        "../../test_data/data.csv"
    );

    EXPECT_EQ(market.getCandles().size(), 1);
    EXPECT_EQ(market.time(),  start_time);

    market.finish();

    EXPECT_NE(market.getCandles().size(), 1);
    EXPECT_EQ(
        market.getCandles().size(),
        linesCount
    );
    EXPECT_EQ(market.time(),  finish_time);
}
