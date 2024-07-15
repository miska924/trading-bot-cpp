#include "markets/clients/tinkoff/utils.h"


namespace TradingBot {
    
    std::string toString(CandleTimeDelta delta) {
        switch (delta) {
            case CANDLE_1_MIN: return "CANDLE_INTERVAL_1_MIN";
            case CANDLE_2_MIN: return "CANDLE_INTERVAL_2_MIN";
            case CANDLE_3_MIN: return "CANDLE_INTERVAL_3_MIN";
            case CANDLE_5_MIN: return "CANDLE_INTERVAL_5_MIN";
            case CANDLE_10_MIN: return "CANDLE_INTERVAL_10_MIN";
            case CANDLE_15_MIN: return "CANDLE_INTERVAL_15_MIN";
            case CANDLE_30_MIN: return "CANDLE_INTERVAL_30_MIN";
            case CANDLE_1_HOUR: return "CANDLE_INTERVAL_HOUR";
            case CANDLE_2_HOUR: return "CANDLE_INTERVAL_2_HOUR";
            case CANDLE_4_HOUR: return "CANDLE_INTERVAL_4_HOUR";
            case CANDLE_1_DAY: return "CANDLE_INTERVAL_DAY";
            case CANDLE_1_WEEK: return "CANDLE_INTERVAL_WEEK";
            case CANDLE_1_MONTH: return "CANDLE_INTERVAL_MONTH";
            default: throw std::runtime_error("Unknown CandleTimeDelta");
        }
    }

    std::string toString(OrderDirection direction) {
        switch (direction) {
            case ORDER_DIRECTION_BUY: return "ORDER_DIRECTION_BUY";
            case ORDER_DIRECTION_SELL: return "ORDER_DIRECTION_SELL";
            default: throw std::runtime_error("Unknown OrderDirection");
        }
    }

} // namespace TradingBot
