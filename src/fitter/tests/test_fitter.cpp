#include <gtest/gtest.h>

#include "fitter/fitter.h"
#include "helpers/test_data.h"
#include "strategies/macd_strategy.h"


const double EPS = 1e-5;
const std::string testDataFileName = "../../../../test_data/btcusdt_15m_3y.csv";
const std::vector<TradingBot::Candle> candles = TradingBot::readCSVFile(testDataFileName);


TEST(TestFitter, TestMACDStrategyFit) {
    TradingBot::StrategyFitter<TradingBot::MACDStrategy> fitter(candles, {1, 1}, {1000, 1000});
    fitter.fit(100);
    fitter.plotBestStrategy("TestMACDStrategyFit.png");
    fitter.heatmapFitnesses("TestMACDStrategyFitHeatmap.png");

    EXPECT_EQ(
        fitter.getBestBalance(),
        43.224088366646342
    );
}

TEST(TestFitter, TestMACDHoldSlowStrategyFit) {
    TradingBot::StrategyFitter<TradingBot::MACDHoldSlowStrategy> fitter(candles, {1, 1}, {1000, 1000});
    fitter.fit(100);
    fitter.plotBestStrategy("TestMACDHoldSlowStrategyFit.png");
    fitter.heatmapFitnesses("TestMACDHoldSlowStrategyFitHeatmap.png");

    EXPECT_EQ(
        fitter.getBestBalance(),
        475.13031729549942
    );
}

TEST(TestFitter, TestMACDHoldFixedStrategyFit) {
    TradingBot::StrategyFitter<TradingBot::MACDHoldFixedStrategy> fitter(candles, {1, 1, 1}, {1000, 1000, 1000});
    fitter.fit(100);
    fitter.plotBestStrategy("TestMACDHoldFixedStrategyFit.png");
    // cannot make heatmap because of too many parameters (3, but need 2)
    
    EXPECT_EQ(
        fitter.getBestBalance(),
        169.10761815812944
    );
}
