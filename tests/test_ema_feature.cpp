#include "gtest/gtest.h"
#include "features/ema_feature.h"
#include "markets/backtest_market.h"


const double EPS = 1e-5;

TEST(EMAFeature, TestEMAFeatureDefault) {
    std::string testDataFileName = "../../test_data/data.csv";
    TradingBot::EMAFeature ema = TradingBot::EMAFeature();
    TradingBot::BacktestMarket market = TradingBot::BacktestMarket(testDataFileName);
    for (int i = 0; i < TradingBot::DEFAULT_EMA_PERIOD - 1; ++i) {
        EXPECT_EQ(ema(market.getCandles()).has_value(), false);
        market.update();
    }
    EXPECT_EQ(ema(market.getCandles()).has_value(), true);
    EXPECT_EQ((*ema(market.getCandles()) - 35275.840330223742) < EPS, true);
}

TEST(EMAFeature, TestEMAFeature30) {
    int period = 30;
    std::string testDataFileName = "../../test_data/data.csv";
    TradingBot::EMAFeature ema = TradingBot::EMAFeature(period);
    TradingBot::BacktestMarket market = TradingBot::BacktestMarket(testDataFileName);
    for (int i = 0; i < period - 1; ++i) {
        EXPECT_EQ(ema(market.getCandles()).has_value(), false);
        market.update();
    }
    EXPECT_EQ(ema(market.getCandles()).has_value(), true);
    EXPECT_EQ((*ema(market.getCandles()) - 35292.009729929006) < EPS, true);
}