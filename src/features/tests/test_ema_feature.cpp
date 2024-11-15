#include <gtest/gtest.h>

#include "features/ema_feature.h"
#include "markets/backtest_market.h"
#include "markets/backtest_market.h"


using namespace TradingBot;
using namespace Helpers;


const double EPS = 1e-5;

std::string testDataFileName = "../../../../test_data/btcusdt_15m_10d.csv";
std::vector<Candle> candles = readCSVFile(testDataFileName);


TEST(EMAFeatureTest, TestEMAFeatureDefault) {
    EMAFeature ema = EMAFeature();
    BacktestMarket market = BacktestMarket(candles);
    for (int i = 0; i < DEFAULT_EMA_PERIOD - 1; ++i) {
        market.update();
    }
    EXPECT_NEAR(ema(market.getCandles()), 34631.606690221779, EPS);
}

TEST(EMAFeatureTest, TestEMAFeature30) {
    int period = 30;
    EMAFeature ema = EMAFeature(period);
    BacktestMarket market = BacktestMarket(candles);
    for (int i = 0; i < period - 1; ++i) {
        market.update();
    }
    EXPECT_NEAR(ema(market.getCandles()), 34647.087373839313, EPS);
}

std::vector<Candle> generateRandomCandles(int numCandles, double minPrice, double maxPrice) {
    std::vector<Candle> candles;
    for (int i = 0; i < numCandles; ++i) {
        double price = minPrice + static_cast<double>(rand()) / RAND_MAX * (maxPrice - minPrice);
        candles.push_back({ .close = price });
    }
    return candles;
}

TEST(EMAFeatureTest, IncrementalVsFullComparison) {
    srand(42);
    const int period = 20;
    const int numCandles = 1000;

    EMAFeature emaIncremental(period);
    EMAFeature emaFull(period);

    std::vector<Candle> candles = generateRandomCandles(numCandles, 80.0, 120.0);
    VectorView<Candle> candleView(candles);

    double result;
    for (int i = period; i <= numCandles; ++i) {
        result = emaIncremental(candleView.subView(0, i), true);
    }
    double fullResult = emaFull(candleView, false);

    EXPECT_NEAR(result, fullResult, EPS);
}
