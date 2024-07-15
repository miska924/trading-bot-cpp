#pragma once

#include "http/client.h"
#include "markets/clients/tinkoff/utils.h"


namespace TradingBot {

    const std::string TINKOFF_ENDPOINT = "https://invest-public-api.tinkoff.ru";
    const std::string TINKOFF_URL_PREFIX = "/rest/tinkoff.public.invest.api.contract.v1.";

    class TinkoffClient{
    public:
        TinkoffClient(
            const std::string& token
        );

        Json::Value InstrumentsService_ShareBy(
            const std::string& id
        );

        Json::Value UsersService_GetAccounts();

        Json::Value MarketDataService_GetCandles(
            const std::string& instrumentId,
            const CandleTimeDelta interval,
            time_t from,
            time_t to
        );

        Json::Value OrdersService_PostOrder(
            uint quantity,
            OrderDirection direction,
            const std::string& accoutId,
            const std::string& instrumentId
        );

        Json::Value OperationsService_GetPositions(const std::string& accountId);

    private:
        HttpClient httpClient;
    };

} // namespace TradingBot
