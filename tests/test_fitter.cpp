#include <gtest/gtest.h>

#include "strategies/macd_strategy.h"
#include "fitter/fitter.h"


const double EPS = 1e-5;

// const std::string testDataFileName = "../../test_data/data.csv";
const std::string testDataFileName = "../../test_data/btcusdt_15m_3y.csv";
const std::vector<TradingBot::Candle> candles = TradingBot::readCSVFile(testDataFileName);


TEST(TestFitter, TestMACDStrategyFit) {
    TradingBot::StrategyFitter<TradingBot::MACDStrategy> fitter(candles);
    fitter.fit(100);
    fitter.plotBestStrategy("TestMACDStrategyFit.png");

    EXPECT_EQ(
        fitter.getBestBalance().asAssetA(),
        43.224088366646342
    );
}

TEST(TestFitter, TestMACDHoldSlowStrategyFit) {
    TradingBot::StrategyFitter<TradingBot::MACDHoldSlowStrategy> fitter(candles);
    fitter.fit(100);
    fitter.plotBestStrategy("TestMACDHoldSlowStrategyFit.png");

    EXPECT_EQ(
        fitter.getBestBalance().asAssetA(),
        475.13031729549942
    );
}

TEST(TestFitter, TestMACDHoldFixedCandlesStrategyFit) {
    TradingBot::StrategyFitter<TradingBot::MACDHoldFixedCandlesStrategy> fitter(candles);
    fitter.fit(100);
    fitter.plotBestStrategy("TestMACDHoldFixedCandlesStrategyFit.png");
    
    EXPECT_EQ(
        fitter.getBestBalance().asAssetA(),
        222.84979384295079
    );
}
