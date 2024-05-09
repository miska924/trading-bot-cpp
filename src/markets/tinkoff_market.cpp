#include "markets/tinkoff_market.h"

#include <iostream>
#include <stdexcept>


namespace TradingBot {

    nlohmann::json TinkoffMarket::Post(const std::string& handler, const std::string& data) {
        std::cerr << handler << "..." << std::endl;
        std::string response;
        if (!connection.Post(
            TINKOFF_SB_API_URL + handler,
            headers,
            data,
            response
        )) {
            throw std::runtime_error(handler + " request failed");
        }
        std::cerr << handler << ": " << response << std::endl;
        return nlohmann::json::parse(response);
    }

    TinkoffMarket::TinkoffMarket() {
        headers = {
            std::string("Authorization: Bearer ") + std::getenv("TINKOFF_SB_TOKEN"),
            "Content-Type: application/json",
            "accept: application/json",
        };

        nlohmann::json accounts = Post("SandboxService/GetSandboxAccounts");

        for (const auto& account: accounts["accounts"]) {
            std::cerr << "account: " << account["id"].get<std::string>() << std::endl;
            Post(
                "SandboxService/CloseSandboxAccount",
                "{ \"accountId\": \"" + account["id"].get<std::string>() + "\" }"
            );
        }

        accountId = Post("SandboxService/OpenSandboxAccount")["accountId"];
        assetA = "rub";
        assetB = "GAZP";

        Post(
            "SandboxService/SandboxPayIn",
            "{"
            "    \"accountId\": \"" + accountId + "\","
            "    \"amount\": {"
            "       \"units\": \"10000\","
            "       \"currency\": \"" + assetA + "\""
            "   }"
            "}"
        );

        instrument = Post(
            "InstrumentsService/ShareBy",
            "{"
            "    \"idType\": \"INSTRUMENT_ID_TYPE_TICKER\","
            "    \"id\": \"GAZP\","
            "    \"classCode\": \"TQBR\""
            "}"
        )["instrument"];
    }

    TinkoffMarket::~TinkoffMarket() {
        std::string response;
        
        if (!connection.Post(
            TINKOFF_SB_API_URL + "SandboxService/CloseSandboxAccount",
            headers,
            "{ \"accountId\": \"" + accountId + "\" }",
            response
        )) {
            std::cerr << "CloseAccount request failed" << std::endl;
        }
    }

    bool TinkoffMarket::order(Order order) {
        if (order.side == OrderSide::RESET) {
            Post(
                "OrdersService/PostOrder",
                "{"
                "    \"quantity\": \"1\","
                "    \"direction\": \"ORDER_DIRECTION_BUY\","
                "    \"accountId\": \"" + accountId + "\","
                "    \"orderType\": \"ORDER_TYPE_MARKET\","
                "    \"instrumentId\": \"" + instrument["uid"].get<std::string>() + "\""
                "}"
            );
        }
        return true;
    }

    bool TinkoffMarket::update() {
        return true;
    }

    Helpers::VectorView<Candle> TinkoffMarket::getCandles() const {
        return candles;
    }

    double TinkoffMarket::GetCurrency(const nlohmann::json& positions, const std::string& assetKey, const std::string& asset) {
        int units = 0;
        int nanoDigits = 0;
        int nano = 0;

        for (const nlohmann::json& position : positions) {
            if (position[assetKey] == asset) {
                units = std::atoi(position["units"].get<std::string>().c_str());
                nanoDigits = std::to_string(abs(position["nano"].get<int>())).size();
                nano = position["nano"].get<int>();
            }
        }

        return units + double(nano) / pow(10, nanoDigits);
    }

    Balance TinkoffMarket::getBalance() {
        nlohmann::json positions = Post(
            "OperationsService/GetPositions",
            "{ \"accountId\": \"" + accountId + "\" }"
        );

        return {
            .assetA = GetCurrency(positions["money"], "currency", assetA),
            .assetB = GetCurrency(positions["securities"], "balance", assetB)
        };
    }

} // namespace TradingBot
