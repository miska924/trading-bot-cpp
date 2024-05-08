#pragma once

#include <curl/curl.h>
#include <string>

#include "markets/market.h"
#include "helpers/vector_view.h"


namespace TradingBot {

    const std::string TINKOFF_API_KEY = std::getenv("TINKOFF_SB_TOKEN");
    const std::string TINKOFF_API_URL = "https://sandbox-invest-public-api.tinkoff.ru/rest/tinkoff.public.invest.api.contract.v1.SandboxService/";

    class TinkoffMarket : public Market {
    public:
        TinkoffMarket();
        ~TinkoffMarket() override;

        bool order(Order order) override;
        bool update() override;
        Helpers::VectorView<Candle> getCandles() const override;
    private:
        std::vector<Candle> candles;
    };

} // namespace TradingBot
