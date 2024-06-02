#include <iostream>
#include <vector>

#include "markets/tinkoff_market.h"
#include "markets/backtest_market.h"
#include "strategies/autofit_strategy.h"
#include "strategies/macd_strategy.h"
#include "strategies/hawks_process_strategy.h"
#include "strategies/averaging_strategy.h"
#include "strategies/dummy_strategy.h"
#include "plotting/plotting.h"
#include "helpers/date_time.h"


std::vector<TradingBot::Candle> getCandles() {
    std::string testDataFileName = "cache_candles.csv";
    std::ifstream file(testDataFileName);
    if (!file.good()) {
        std::cerr << "File " << testDataFileName << " not found" << std::endl;
        TradingBot::TinkoffMarket market(
            TradingBot::TinkoffMarketMode::SB,
            30000,
            TradingBot::CandleTimeDelta::CANDLE_15_MIN,
            "AFLT",
            2
        );
        std::vector<TradingBot::Candle> candles = market.getCandles().toVector();
        std::ofstream outFile(testDataFileName);
        outFile << ",time,open,high,low,close,volume" << std::endl;
        for (auto candle : candles) {
            outFile << Helpers::DateTime(candle.time) << "," 
                    << candle.time << ","
                    << candle.open << ","
                    << candle.high << ","
                    << candle.low << ","
                    << candle.close << ","
                    << candle.volume << std::endl;
        }
        outFile.close();
    }

    return TradingBot::readCSVFile(testDataFileName);
}

void test() {
    std::vector<TradingBot::Candle> candles = getCandles();
    TradingBot::BacktestMarket backtestMarket(candles, TradingBot::CandleTimeDelta::CANDLE_15_MIN, true, true);
    std::cerr << "Loaded candles" << std::endl;

    // TradingBot::MACDHoldFixedStrategy strategy(&backtestMarket, 10, 300, 300);

    // TradingBot::AutoFitStrategy<TradingBot::AveragingStrategy> strategy(
    //     &backtestMarket,
    //     {10000, 0, 1000, 100000, 0, 1.0},
    //     {100, 1, 100, 3.0, 0.1},
    //     {1000, 100, 3000, 40.0, 0.1}
    // );
    // TradingBot::AutoFitStrategy<TradingBot::HawksProcessStrategy> strategy(
    //     &backtestMarket,
    //     3000, // fitWindow
    //     0, // testWindow
    //     1000, // fitStep
    //     100, // fitIterations
    //     0, // forceStop
    //     1, // fitAroundThreshold
    //     {
    //         1000, /* atrPeriod */ 
    //         1000, /* normRangePeriod */ 
    //         10, /* normalRangeSmoothPeriod */ 
    //         1.0, /* risk */ 
    //         1, /* preventDrawdown */ 
    //         0.01 /* preventDrawdownCoeff */
    //     },
    //     {
    //         1000, /* atrPeriod */ 
    //         1000, /* normRangePeriod */ 
    //         100, /* normalRangeSmoothPeriod */ 
    //         1.0, /* risk */ 
    //         1, /* preventDrawdown */ 
    //         0.01 /* preventDrawdownCoeff */
    //     }
    // );

    TradingBot::HawksProcessStrategy strategy(
        &backtestMarket,
        5000, /* atrPeriod */ 
        5000, /* normRangePeriod */ 
        1, /* normalRangeSmoothPeriod */ 
        1.0, /* risk */ 
        1, /* preventDrawdown */ 
        0.01 /* preventDrawdownCoeff */
    );
    strategy.run();
    plot("test.png", backtestMarket.getCandles(), backtestMarket.getOrderHistory(), backtestMarket.getBalanceHistory());
}


int main() {
    TradingBot::TinkoffMarket market(
        TradingBot::TinkoffMarketMode::PROD,
        400,
        TradingBot::CandleTimeDelta::CANDLE_15_MIN,
        "GAZP",
        2
    );
    TradingBot::DummyStrategy strategy(&market);
    strategy.run();
    // test();
    return 0;
}
