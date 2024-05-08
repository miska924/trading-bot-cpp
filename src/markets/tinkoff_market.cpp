#include "markets/tinkoff_market.h"


namespace TradingBot {

    TinkoffMarket::TinkoffMarket() {
        curl_global_init(CURL_GLOBAL_ALL);
    }

    TinkoffMarket::~TinkoffMarket() {
        curl_global_cleanup();
    }

    bool TinkoffMarket::order(Order order) {
        return true;
    }

    bool TinkoffMarket::update() {
        return true;
    }


    Helpers::VectorView<Candle> TinkoffMarket::getCandles() const {
        return candles;
    }

} // namespace TradingBot
