#include "markets/tinkoff_market.h"

#include <iostream>
#include <stdexcept>


namespace TradingBot {

    time_t ParseDateTime(const char* datetimeString)
    {
        struct tm tmStruct;
        strptime(datetimeString, DATETIME_FORMAT.c_str(), &tmStruct);
        return mktime(&tmStruct);
    }
    
    std::string DateTime(time_t time)
    {
        char buffer[90];
        struct tm* timeinfo = localtime(&time);
        strftime(buffer, sizeof(buffer), DATETIME_FORMAT.c_str(), timeinfo);
        return buffer;
    }

    double doubleFromTinfoffFormat(const nlohmann::json& dict) {
        int units = std::atoi(dict["units"].get<std::string>().c_str());
        int nanoDigits = std::to_string(abs(dict["nano"].get<int>())).size();
        int nano = dict["nano"].get<int>();
        return units + double(nano) / pow(10, nanoDigits);
    }

    nlohmann::json TinkoffMarket::Post(const std::string& handler, const std::string& data) {
        std::cerr << handler << "..." << nlohmann::json::parse(data).dump(2) << std::endl;
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

        CloseAllAccounts();

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

        LoadCandlesHistory();
    }

    void TinkoffMarket::CloseAllAccounts() {
        nlohmann::json accounts = Post("SandboxService/GetSandboxAccounts");

        for (const auto& account: accounts["accounts"]) {
            std::cerr << "account: " << account["id"].get<std::string>() << std::endl;
            Post(
                "SandboxService/CloseSandboxAccount",
                "{ \"accountId\": \"" + account["id"].get<std::string>() + "\" }"
            );
        }
    }

    void TinkoffMarket::LoadCandlesHistory() {
        time_t to = std::time(nullptr);
        time_t from = to - 60 * 60;

        nlohmann::json historyResponse = Post(
            "MarketDataService/GetCandles",
            "{"
            "    \"instrumentId\": \"" + instrument["uid"].get<std::string>() + "\","
            "    \"interval\": \"CANDLE_INTERVAL_1_MIN\","
            "    \"from\": \"" + DateTime(from) + "\","
            "    \"to\": \"" + DateTime(to) + "\""
        )["candles"];

        for (const auto& candle: historyResponse) {      

            candles.push_back(Candle{
                .time = ParseDateTime(candle["time"].get<std::string>().c_str()),
                .open = doubleFromTinfoffFormat(candle["open"]),
                .high = doubleFromTinfoffFormat(candle["high"]),
                .low = doubleFromTinfoffFormat(candle["low"]),
                .close = doubleFromTinfoffFormat(candle["close"]),
                .volume = std::stod(candle["volume"].get<std::string>())
            });
        }
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
            if (balance.assetB == 0) {
                return true;
            }
            Post(
                "OrdersService/PostOrder",
                "{"
                "    \"quantity\": \"" + std::to_string(abs(balance.assetB)) + "\","
                "    \"direction\": \"" + (
                    balance.assetB > 0 ? "ORDER_DIRECTION_SELL" : "ORDER_DIRECTION_BUY"
                ) + "\","
                "    \"accountId\": \"" + accountId + "\","
                "    \"orderType\": \"ORDER_TYPE_MARKET\","
                "    \"instrumentId\": \"" + instrument["uid"].get<std::string>() + "\""
                "}"
            );
        } else if (order.side == OrderSide::BUY) {
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
        } else if (order.side == OrderSide::SELL) {
            Post(
                "OrdersService/PostOrder",
                "{"
                "    \"quantity\": \"1\","
                "    \"direction\": \"ORDER_DIRECTION_SELL\","
                "    \"accountId\": \"" + accountId + "\","
                "    \"orderType\": \"ORDER_TYPE_MARKET\","
                "    \"instrumentId\": \"" + instrument["uid"].get<std::string>() + "\""
                "}"
            );
        }
        balance = getBalance();
        lastOrder = order;
        return true;
    }

    bool TinkoffMarket::update() {
        // проверить расписание и вернуть false, если сейчас не идут торги
        // проверить, появилась ли полноценная новая свеча и вернуть false, если нет
        // получить свечи
        return true;
    }

    Helpers::VectorView<Candle> TinkoffMarket::getCandles() const {
        return candles;
    }

    double TinkoffMarket::GetCurrency(const nlohmann::json& positions, const std::string& assetKey, const std::string& asset) {
        for (const nlohmann::json& position : positions) {
            if (position[assetKey] == asset) {
                return doubleFromTinfoffFormat(position);
            }
        }
        return 0;
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
