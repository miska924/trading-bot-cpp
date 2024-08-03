#include <gtest/gtest.h>

#include "features/atr_feature.h"


using namespace TradingBot;

const double EPS = 1e-5;


TEST(ATRFeatureTest, TestATRFeature) {
    ATRFeature atr(5);
    std::vector<Candle> candles = {
        { .high = 100000, .low = 0 },
        { .high = 100, .low = 90 },
        { .high = 100, .low = 90 },
        { .high = 100, .low = 90 },
        { .high = 100, .low = 90 },
        { .high = 100, .low = 90 },
    };
    EXPECT_EQ(atr(candles, true), 10);
    candles.push_back({ .high = 100, .low = 90 });
    EXPECT_EQ(atr(candles, true), 10);
}