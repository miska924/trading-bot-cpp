#include <gtest/gtest.h>

#include "features/sma_feature.h"


using namespace TradingBot;

const double EPS = 1e-5;


TEST(SMAFeatureTest, TestSMAFeature) {
    SMAFeature sma(5);
    std::vector<Candle> candles = {
        { .close = -1000},
        { .close = 1 },
        { .close = 2 },
        { .close = 3 },
        { .close = 4 },
        { .close = 5 },
    };
    EXPECT_EQ(sma(candles, true), 3);
    candles.push_back(
        { .close = 6 }
    );
    EXPECT_EQ(sma(candles, true), 4);
}