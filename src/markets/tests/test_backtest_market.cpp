#include <gtest/gtest.h>
#include <fstream>

#include "markets/backtest_market.h"



std::string testDataFileName = "../../../../test_data/data.csv";
std::vector<TradingBot::Candle> candles = TradingBot::readCSVFile(testDataFileName);

// TEST(BacktestingTest, TestStartTime) {
//     TradingBot::BacktestMarket market = TradingBot::BacktestMarket();
//     EXPECT_EQ(market.time(), 0);
// }

// TEST(BacktestingTest, TestHistory) {
//     TradingBot::BacktestMarket market = TradingBot::BacktestMarket();

//     std::vector<TradingBot::Order> orders = {
//         {
//             .side = TradingBot::OrderSide::BUY,
//             .amount = 1
//         },
//         {
//             .side = TradingBot::OrderSide::SELL,
//             .amount = 10.3
//         },
//     };

//     EXPECT_EQ(market.getOrderHistory().size(), 0);

//     for (const auto& order : orders) {
//         market.order(order);
//     }
    
//     EXPECT_EQ(market.getOrderHistory().size(), 2);
//     EXPECT_EQ(market.getOrderHistory()[0], orders[0]);
//     EXPECT_EQ(market.getOrderHistory()[1], orders[1]);
// }

// TEST(BacktestingTest, TestRandomCandlesGeneration) {
//     TradingBot::BacktestMarket market = TradingBot::BacktestMarket();
//     EXPECT_EQ(market.getCandles().size(), 1);
//     market.finish();
//     EXPECT_EQ(
//         market.getCandles().size(),
//         TradingBot::DEFAULT_BACKTEST_MARKET_SIZE
//     );
// }

// TEST(BacktestingTest, TestCandleTimeDelta) {
//     TradingBot::BacktestMarket market = TradingBot::BacktestMarket();
//     EXPECT_EQ(market.getCandleTimeDelta(), 1);
// }

TEST(BacktestingTest, TestReadCSVCandle) {
    std::string csvCandle = "2023-11-08 06:00:00,1699412400.0,35306.61,35321.37,35260.0,35288.65,182.86608";
    TradingBot::Candle candle = TradingBot::readCSVCandle(csvCandle);
    EXPECT_EQ(candle, TradingBot::Candle({
        .time = 1699412400,
        .open = 35306.61,
        .high = 35321.37,
        .low = 35260.0,
        .close = 35288.65,
        .volume = 182.86608
    }));
}

void testCandlesFromFile(const std::string& testDataFileName) {
    std::ifstream file(testDataFileName);
    
    size_t linesCount = 0;
    time_t start_time = 0;
    time_t finish_time = 0;
    std::string line;
    std::getline(file, line); // skip header
    bool first = true;
    while (std::getline(file, line)) {
        linesCount++;
        finish_time = TradingBot::readCSVCandle(line).time;
        if (first) {
            first = false;
            start_time = finish_time;
        }
    }
    file.close();

    TradingBot::BacktestMarket market = TradingBot::BacktestMarket(
        candles
    );

    EXPECT_EQ(market.getCandles().size(), 1);
    EXPECT_EQ(market.time(),  start_time);

    market.finish();

    EXPECT_NE(market.getCandles().size(), 1);
    EXPECT_EQ(
        market.getCandles().size(),
        linesCount
    );
    EXPECT_EQ(market.time(), finish_time);
}

TEST(BacktestingTest, TestCandlesFromFile) {
    testCandlesFromFile(testDataFileName);
}

TEST(BacktestingTest, TestCandleTimeDelta15m) {
    TradingBot::BacktestMarket market = TradingBot::BacktestMarket(
        candles
    );
    EXPECT_EQ(market.getCandleTimeDelta(), 15 * 60);
}

TEST(BacktestingTest, TestCandlesFromLargeFile) {
    std::string testDataFileName = "../../../../test_data/data.csv";
    testCandlesFromFile(testDataFileName);
}
