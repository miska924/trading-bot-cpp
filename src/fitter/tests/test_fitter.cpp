#include <gtest/gtest.h>

#include "strategies/macd_strategy.h"
#include "fitter/fitter.h"
#include "helpers/test_data.h"


const double EPS = 1e-5;

const std::string testDataFileName = "../../../../test_data/btcusdt_15m_3y.csv";
const std::vector<TradingBot::Candle> candles = TradingBot::readCSVFile(testDataFileName);


TEST(TestFitter, TestMACDStrategyFit) {
    TradingBot::StrategyFitter<TradingBot::MACDStrategy> fitter(candles, {1, 1}, {1000, 1000});
    fitter.fit(100);
    fitter.plotBestStrategy("TestMACDStrategyFit.png");

    EXPECT_EQ(
        fitter.getBestBalance(),
        43.224088366646342
    );
}

TEST(TestFitter, TestMACDHoldSlowStrategyFit) {
    TradingBot::StrategyFitter<TradingBot::MACDHoldSlowStrategy> fitter(candles, {1, 1}, {1000, 1000});
    fitter.fit(100);
    fitter.plotBestStrategy("TestMACDHoldSlowStrategyFit.png");

    EXPECT_EQ(
        fitter.getBestBalance(),
        475.13031729549942
    );
}

TEST(TestFitter, TestMACDHoldFixedStrategyFit) {
    TradingBot::StrategyFitter<TradingBot::MACDHoldFixedStrategy> fitter(candles, {1, 1, 1}, {1000, 1000, 1000});
    fitter.fit(100);
    fitter.plotBestStrategy("TestMACDHoldFixedStrategyFit.png");
    
    EXPECT_EQ(
        fitter.getBestBalance(),
        222.84979384295079
    );
}
