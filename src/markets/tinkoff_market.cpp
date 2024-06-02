#include "markets/tinkoff_market.h"
#include "helpers/date_time.h"

#include <iostream>
#include <ctime>
#include <stdexcept>
#include <thread>
#include <math.h>
#include <fmt/core.h>


namespace TradingBot {

    std::string toTinkoffString(CandleTimeDelta delta) {
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
            default: throw std::runtime_error("Unknown CANDLE_TIMEDELTA");
        }
    }

    double doubleFromTinkoffFormat(const nlohmann::json& dict) {
        int units = std::atoi(dict["units"].get<std::string>().c_str());
        int nanoDigits = std::to_string(abs(dict["nano"].get<int>())).size();
        int nano = dict["nano"].get<int>();
        return units + double(nano) / pow(10, nanoDigits);
    }

    nlohmann::json TinkoffMarket::Post(const std::string& handler, nlohmann::json data) {
        if (verbose >= 2) {
            std::cerr << handler << "..." << data.dump(2) << std::endl;
        }
        
        nlohmann::json result;
        while (true) {
            std::string response;
            if (!connection.Post(
                (mode == TinkoffMarketMode::SB ? TINKOFF_SB_API_URL : TINKOFF_API_URL) + handler,
                headers,
                data.dump(),
                response
            )) {
                if (verbose >= 2) {
                    std::cerr << handler << " request failed" << std::endl;
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue; // throw std::runtime_error(handler + " request failed");
            }

            if (response.empty()) {
                if (verbose >= 2) {
                    std::cerr << "empty response" << std::endl;
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue; // throw std::runtime_error(handler + " empty response");
            }

            result = nlohmann::json::parse(response);

            if (result.contains("code") && result["code"] != 0) {
                std::cerr << handler << ": " << response << std::endl;
                throw std::runtime_error(handler + " error code " + result["code"].dump());
            }
            break;
        }

        return result;
    }

    TinkoffMarket::TinkoffMarket(
        TinkoffMarketMode mode,
        size_t candlesCount,
        CandleTimeDelta candleTimeDelta,
        std::string asset,
        int verbose
    ) : verbose(verbose), candlesCount(candlesCount), timeDelta(candleTimeDelta), assetB(asset), mode(mode) {
        headers = {
            std::string("Authorization: Bearer ") + (mode == TinkoffMarketMode::SB ? std::getenv("TINKOFF_SB_TOKEN") : std::getenv("TINKOFF_TOKEN")),
            "Content-Type: application/json",
            "accept: application/json",
        };

        assetA = "rub";

        if (mode == TinkoffMarketMode::SB) {
            CloseAllAccounts();
            accountId = Post("SandboxService/OpenSandboxAccount")["accountId"];
            Post(
                "SandboxService/SandboxPayIn",
                {
                    {"accountId", accountId},
                    {"amount", {
                        {"units", "10000"},
                        {"currency", assetA}
                    }}
                }
            );
        } else {
            // TODO: resolve from api
            accountId = Post("UserService/GetAccounts")["accounts"][0]["id"];
        }

        instrument = Post(
            "InstrumentsService/ShareBy",
            {
                {"idType", "INSTRUMENT_ID_TYPE_TICKER"},
                {"id", assetB},
                {"classCode", "TQBR"}
            }
        )["instrument"];

        this->candleTimeDelta = timeDelta;
        candleTimeDeltaString = toTinkoffString(timeDelta);
        LoadCandlesHistory();

        if (verbose >= 1) {
            std::cerr << "TinkoffMarket: candles loaded: " << candles.size() << std::endl;
        }
    }

    void TinkoffMarket::CloseAllAccounts() {
        if (mode != TinkoffMarketMode::SB) {
            return;
        }

        nlohmann::json accounts = Post("SandboxService/GetSandboxAccounts");

        for (const auto& account: accounts["accounts"]) {
            Post(
                "SandboxService/CloseSandboxAccount",
                {
                    {"accountId", account["id"]}
                }
            );
        }
    }

    bool TinkoffMarket::LoadCandlesHistory() {
        const bool init = candles.empty();
        time_t to = std::time(nullptr);
        time_t from = (init ? to - candleTimeDelta * this->candlesCount : candles.back().time);
        int partCount = (to - from + HISTORY_BATCH_SIZE * candleTimeDelta - 1) / (HISTORY_BATCH_SIZE * candleTimeDelta);

        bool updated = false;
        int part = 0;
        bool wasNotCompletedCandle = false;
        while ((init && candles.size() < this->candlesCount) || (!init && part < partCount)) {
            if (init && verbose >= 1) {
                std::cerr << "TinkoffMarket: candles loading: " << candles.size() <<  "\t out of " << this->candlesCount << std::endl;
            }
            nlohmann::json historyResponse = Post(
                "MarketDataService/GetCandles",
                {
                    {"instrumentId", instrument["uid"]},
                    {"interval", candleTimeDeltaString},
                    {"from", Helpers::DateTime(
                        (init ? to - (part + 1) * HISTORY_BATCH_SIZE * candleTimeDelta : from + part * HISTORY_BATCH_SIZE * candleTimeDelta)
                    )},
                    {"to", Helpers::DateTime(
                        int((init ? to - part * HISTORY_BATCH_SIZE * candleTimeDelta : from + (part + 1) * HISTORY_BATCH_SIZE * candleTimeDelta))
                    )}
                }
            )["candles"];

            if (historyResponse.empty()) {
                ++part;
                continue;
            }

            for (int i = (init ? historyResponse.size() - 1 : 0); (init ? i >= 0 : i < historyResponse.size()); (init ? --i : ++i)) {
                if (init && candles.size() >= this->candlesCount) {
                    break;
                }
                const auto& candle = historyResponse[i];
                time_t candleTime = Helpers::ParseDateTime(candle["time"].get<std::string>().c_str());
                if (!candles.empty() && (
                    (init && candles.front().time <= candleTime) ||
                    (!init && candles.back().time >= candleTime))) {
                    continue;
                }
                if (!candle.contains("isComplete") || !candle["isComplete"].get<bool>()) {
                    if (!init) {
                        wasNotCompletedCandle = true;
                    }
                    continue;
                }
                updated = true;
                Candle pushCandle = {
                    .time = candleTime,
                    .open = doubleFromTinkoffFormat(candle["open"]),
                    .high = doubleFromTinkoffFormat(candle["high"]),
                    .low = doubleFromTinkoffFormat(candle["low"]),
                    .close = doubleFromTinkoffFormat(candle["close"]),
                    .volume = std::stod(candle["volume"].get<std::string>())
                };
                candles.push_back(pushCandle);
            }
            ++part;
        }
        if (init) {
            reverse(candles.begin(), candles.end());
        }
        if (this->candlesCount * 2 < candles.size()) {
            int toRemove = candles.size() - this->candlesCount;
            candles = std::vector(
                candles.begin() + toRemove,
                candles.end()
            );
        }
        return updated;
    }

    TinkoffMarket::~TinkoffMarket() {
        if (mode != TinkoffMarketMode::SB) {
            return;
        }

        Post(
            "SandboxService/CloseSandboxAccount",
            {
                {"accountId", accountId}
            }
        );
    }

    bool TinkoffMarket::order(Order order) {
        if (order.side == OrderSide::RESET) {
            if (balance.assetB == 0) {
                return true;
            }
            Post(
                "OrdersService/PostOrder",
                {
                    {"quantity", std::to_string(abs(balance.assetB))},
                    {"direction", balance.assetB > 0 ? "ORDER_DIRECTION_SELL" : "ORDER_DIRECTION_BUY"},
                    {"accountId", accountId},
                    {"orderType", "ORDER_TYPE_MARKET"},
                    {"instrumentId", instrument["uid"]}
                }
            );
        } else if (order.side == OrderSide::BUY) {
            Post(
                "OrdersService/PostOrder",
                {
                    {"quantity", "1"},
                    {"direction", "ORDER_DIRECTION_BUY"},
                    {"accountId", accountId},
                    {"orderType", "ORDER_TYPE_MARKET"},
                    {"instrumentId", instrument["uid"]}
                }
            );
        } else if (order.side == OrderSide::SELL) {
            Post(
                "OrdersService/PostOrder",
                {
                    {"quantity", "1"},
                    {"direction", "ORDER_DIRECTION_SELL"},
                    {"accountId", accountId},
                    {"orderType", "ORDER_TYPE_MARKET"},
                    {"instrumentId", instrument["uid"]}
                }
            );
        }
        balance = getBalance();
        lastOrder = order;
        return true;
    }

    bool TinkoffMarket::update() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        bool result = LoadCandlesHistory();
        // if (!Post(
        //     "MarketDataService/GetTradingStatus",
        //     {
        //         {"instrumentId", instrument["uid"]}
        //     }
        // )["marketOrderAvailableFlag"]) {
        if (result) {
            std::cerr << getBalance().asAssetA() << std::endl;
        }
        return result;
    }

    Helpers::VectorView<Candle> TinkoffMarket::getCandles() const {
        return candles;
    }

    double TinkoffMarket::GetCurrency(const nlohmann::json& positions, const std::string& assetKey, const std::string& asset) {
        for (const nlohmann::json& position : positions) {
            if (position.contains(assetKey) && position[assetKey] == asset) {
                return doubleFromTinkoffFormat(position);
            }
        }
        return 0;
    }

    Balance TinkoffMarket::getBalance() {
        nlohmann::json positions = Post(
            "OperationsService/GetPositions",
            {
                {"accountId", accountId}
            }
        );
        return balance = {
            .assetA = GetCurrency(positions["money"], "currency", assetA),
            .assetB = GetCurrency(positions["securities"], "balance", assetB)
        };
    }

} // namespace TradingBot
