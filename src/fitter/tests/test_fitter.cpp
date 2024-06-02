#include <gtest/gtest.h>
#include <iostream>

#include "fitter/fitter.h"
#include "helpers/test_data.h"
#include "strategies/strategy.h"
#include "strategies/macd_strategy.h"
#include "strategies/averaging_strategy.h"


const double EPS = 1e-5;
const std::string testDataFileName = "../../../../test_data/btcusdt_15m_3y.csv";
const std::vector<TradingBot::Candle> candles = TradingBot::readCSVFile(testDataFileName);


TEST(TestFitter, TestMACDStrategyFit) {
    TradingBot::StrategyFitter<TradingBot::MACDStrategy> fitter(candles, TradingBot::TEST_CANDLES_TIMEDELTA, {1, 1}, {1000, 1000});
    fitter.fit(100);
    fitter.plotBestStrategy("TestMACDStrategyFit.png");
    fitter.heatmapFitnesses("TestMACDStrategyFitHeatmap.png");

    EXPECT_EQ(
        fitter.getBestBalance(),
        156.02999080723393
    );
}

TEST(TestFitter, TestMACDHoldSlowStrategyFit) {
    TradingBot::StrategyFitter<TradingBot::MACDHoldSlowStrategy> fitter(candles, TradingBot::TEST_CANDLES_TIMEDELTA, {1, 1}, {1000, 1000});
    fitter.fit(100);
    fitter.plotBestStrategy("TestMACDHoldSlowStrategyFit.png");
    fitter.heatmapFitnesses("TestMACDHoldSlowStrategyFitHeatmap.png");

    EXPECT_EQ(
        fitter.getBestBalance(),
        463.7644542941091
    );
}

TEST(TestFitter, TestMACDHoldSlowStrategyFitTrainTest) {
    int BEGIN = 5000;
    int TRAIN = 10000;
    int STEP1 = 2000;
    int STEP2 = 1000;
    Helpers::VectorView<TradingBot::Candle> train(candles, BEGIN, BEGIN + TRAIN);
    Helpers::VectorView<TradingBot::Candle> test(candles, BEGIN + TRAIN, BEGIN + TRAIN + STEP1);
    Helpers::VectorView<TradingBot::Candle> test2(candles, BEGIN + TRAIN + STEP1, BEGIN + TRAIN + STEP1 + STEP2);
    TradingBot::StrategyFitter<TradingBot::MACDHoldSlowStrategy> fitter(train, TradingBot::TEST_CANDLES_TIMEDELTA, {1, 1}, {1000, 1000});
    fitter.fit(100);
    fitter.plotBestStrategy("TestMACDHoldSlowStrategyFit.png");
    fitter.heatmapFitnesses("TestMACDHoldSlowStrategyFitHeatmap.png");

    EXPECT_EQ(
        fitter.getBestBalance(),
        174.67593979808686
    );

    fitter.test(test);
    fitter.heatmapTestFitnesses("TestMACDHoldSlowStrategyFitHeatmapTest.png");
    fitter.test(test2);
    fitter.heatmapTestFitnesses("TestMACDHoldSlowStrategyFitHeatmapTest2.png");
}

TEST(TestFitter, TestMACDHoldFixedStrategyFit) {
    TradingBot::StrategyFitter<TradingBot::MACDHoldFixedStrategy> fitter(candles, TradingBot::TEST_CANDLES_TIMEDELTA, {1, 1, 1}, {1000, 1000, 1000});
    fitter.fit(100);
    fitter.plotBestStrategy("TestMACDHoldFixedStrategyFit.png");
    // cannot make heatmap because of too many parameters (3, but need 2)
    
    EXPECT_EQ(
        fitter.getBestBalance(),
        160.82584865112196
    );
}


TEST(TestFitter, TestAveragingStrategyFit) {
    TradingBot::StrategyFitter<TradingBot::AveragingStrategy> fitter(candles, TradingBot::TEST_CANDLES_TIMEDELTA, {100, 51, 775, 40.6, 0.1}, {100, 51, 775, 40.6, 5.0}, 1);
    fitter.fit(1000);
    std::cerr << fitter.getBestParameters() << std::endl;
    fitter.plotBestStrategy("TestAveragingStrategyFit.png");
    // fitter.heatmapFitnesses("TestAveragingStrategyFitHeatmap.png");
    // cannot make heatmap because of too few parameters (1, but need 2)
    
    EXPECT_EQ(
        fitter.getBestBalance(),
        107674.87292014458 // it's actually overfitting i think
    );
}
// 100 51 775 40.6 0.1