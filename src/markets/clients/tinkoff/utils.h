#pragma once

#include <string>
#include <stdexcept>


namespace TradingBot {

    enum CandleTimeDelta {
        CANDLE_1_MIN = 60,
        CANDLE_2_MIN = 2 * CANDLE_1_MIN,
        CANDLE_3_MIN = 3 * CANDLE_1_MIN,
        CANDLE_5_MIN = 5 * CANDLE_1_MIN,
        CANDLE_10_MIN = 10 * CANDLE_1_MIN,
        CANDLE_15_MIN = 15 * CANDLE_1_MIN,
        CANDLE_30_MIN = 30 * CANDLE_1_MIN,
        CANDLE_1_HOUR = 60 * CANDLE_1_MIN,
        CANDLE_2_HOUR = 2 * CANDLE_1_HOUR,
        CANDLE_4_HOUR = 4 * CANDLE_1_HOUR,
        CANDLE_1_DAY = 24 * CANDLE_1_MIN,
        CANDLE_1_WEEK = 7 * CANDLE_1_DAY,
        CANDLE_1_MONTH = 30 * CANDLE_1_DAY,
    };

    enum OrderDirection {
        ORDER_DIRECTION_BUY = 0,
        ORDER_DIRECTION_SELL = 1
    };

    std::string toString(CandleTimeDelta delta);
    std::string toString(OrderDirection direction);

} // namespace TradingBot
