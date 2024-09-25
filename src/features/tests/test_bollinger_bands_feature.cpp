#include <cmath>
#include <cstdlib>
#include <gtest/gtest.h>
#include <vector>

#include "features/bollinger_bands_feature.h"
#include "features/sma_feature.h"

using namespace TradingBot;

const double EPS = 1e-5;

TEST(BollingerBandsFeatureTest, BasicTest) {
    BollingerBandsFeature bb(5, 2.0);
    SMAFeature sma(5);

    std::vector<Candle> candles = {
        { .close = 100 },
        { .close = 100 },
        { .close = 100 },
        { .close = 100 },
        { .close = 100 },
    };

    EXPECT_NEAR(bb(candles, false), 0.0, EPS);
}

TEST(BollingerBandsFeatureTest, VolatileDataTest) {
    BollingerBandsFeature bb(5, 2.0);
    SMAFeature sma(5);

    std::vector<Candle> candles = {
        { .close = 95 },
        { .close = 105 },
        { .close = 100 },
        { .close = 110 },
        { .close = 90 },
    };

    double expectedSMA = sma(candles, false);

    double sum = 0;
    for (const auto& candle : candles) {
        sum += std::pow(candle.close - expectedSMA, 2);
    }
    double stddev = std::sqrt(sum / candles.size());

    EXPECT_NEAR(bb(candles, false), 2.0 * stddev, EPS);
}

TEST(BollingerBandsFeatureTest, IncrementalUpdateTest) {
    BollingerBandsFeature bb(5, 2.0);

    std::vector<Candle> candles = {
        { .close = 95 },
        { .close = 105 },
        { .close = 100 },
        { .close = 110 },
        { .close = 90 },
    };

    double initialWidth = bb(candles, false);

    candles.push_back({ .close = 100 });

    EXPECT_NEAR(bb(candles, true), bb(candles, false), EPS);
}

TEST(BollingerBandsFeatureTest, LargeConstantDataTest) {
    BollingerBandsFeature bb(10, 2.0);

    std::vector<Candle> candles(10, { .close = 100 });

    EXPECT_NEAR(bb(candles, false), 0.0, EPS);
}

TEST(BollingerBandsFeatureTest, ShortPeriodTest) {
    BollingerBandsFeature bb(2, 2.0);

    std::vector<Candle> candles = {
        { .close = 100 },
        { .close = 105 },
    };

    double sma = (100 + 105) / 2.0;
    double stddev = std::sqrt((std::pow(100 - sma, 2) + std::pow(105 - sma, 2)) / 2.0);
    EXPECT_NEAR(bb(candles, false), 2.0 * stddev, EPS);
}

std::vector<Candle> generateRandomCandles(int numCandles, double minPrice, double maxPrice) {
    std::vector<Candle> candles;
    for (int i = 0; i < numCandles; ++i) {
        double price = minPrice + static_cast<double>(rand()) / RAND_MAX * (maxPrice - minPrice);
        candles.push_back({ .close = price });
    }
    return candles;
}

TEST(BollingerBandsFeatureTest, IncrementalVsFullComparison) {
    srand(42);
    const int period = 20;
    const double k = 2.0;
    const int numCandles = 1000;

    BollingerBandsFeature bbIncremental(period, k);
    BollingerBandsFeature bbFull(period, k);

    std::vector<Candle> candles = generateRandomCandles(numCandles, 80.0, 120.0);
    Helpers::VectorView<Candle> candleView(candles);

    double result;
    for (int i = period; i <= numCandles; ++i) {
        result = bbIncremental(candleView.subView(0, i), true);
    }
    double fullResult = bbFull(candleView, false);

    EXPECT_NEAR(result, fullResult, EPS);
}
