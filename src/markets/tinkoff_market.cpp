#include "markets/tinkoff_market.h"
#include "helpers/date_time.h"

#include <iostream>
#include <ctime>
#include <stdexcept>
#include <thread>
#include <math.h>


namespace TradingBot {

    double doubleFromTinkoffFormat(const Json::Value& dict) {
        int units = std::atoi(dict["units"].asString().c_str());
        int nanoDigits = std::to_string(abs(dict["nano"].asInt())).size();
        int nano = dict["nano"].asInt();
        return units + double(nano) / pow(10, nanoDigits);
    }

    std::string resolveAccountId(TinkoffClient& tinkoffClient) {
        std::string result;
        bool got = false;
        Json::Value accounts = tinkoffClient.UsersService_GetAccounts()["accounts"];

        for (const auto& account : accounts) {
            if (account["type"] == "ACCOUNT_TYPE_TINKOFF") {
                result = account["id"].asString();
                got = true;
                break;
            }
        }
        if (!got) {
            throw std::runtime_error("TinkoffMarket: Tinkoff account with ACCOUNT_TYPE_TINKOFF not found");
        }
        std::cerr << "accountId: " << result << std::endl;
        return result;
    }

    TinkoffMarket::TinkoffMarket(
        size_t candlesCount,
        CandleTimeDelta candleTimeDelta,
        std::string asset,
        int verbose
    ) :
        tinkoffClient(std::getenv("TINKOFF_TOKEN")),
        verbose(verbose),
        candlesCount(candlesCount),
        timeDelta(candleTimeDelta),
        assetA("rub"),
        assetB(asset)
    {
        fee = 0.003;
        accountId = resolveAccountId(tinkoffClient);
        instrument = tinkoffClient.InstrumentsService_ShareBy(assetB)["instrument"];

        this->candleTimeDelta = timeDelta;
        candleTimeDeltaString = toString(timeDelta);
        LoadCandlesHistory();
        updateBalance();
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
            Json::Value historyResponse = tinkoffClient.MarketDataService_GetCandles(
                instrument["uid"].asString(),
                timeDelta,
                int(init ? to - (part + 1) * HISTORY_BATCH_SIZE * candleTimeDelta : from + part * HISTORY_BATCH_SIZE * candleTimeDelta),
                int((init ? to - part * HISTORY_BATCH_SIZE * candleTimeDelta : from + (part + 1) * HISTORY_BATCH_SIZE * candleTimeDelta))
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
                time_t candleTime = Helpers::ParseDateTime(candle["time"].asString().c_str());
                if (!candles.empty() && (
                    (init && candles.front().time <= candleTime) ||
                    (!init && candles.back().time >= candleTime))) {
                    continue;
                }
                if (!candle["isComplete"] || !candle["isComplete"].asBool()) {
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
                    .volume = std::stod(candle["volume"].asString())
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

        if (verbose >= 1 && updated) {
            std::cerr << "TinkoffMarket: candles loaded: " << candles.size() << std::endl;
        }

        return updated;
    }

    void TinkoffMarket::performOrder(double amount, OrderDirection direction) {
        tinkoffClient.OrdersService_PostOrder(
            amount,
            direction,
            accountId,
            instrument["uid"].asString()
        );
    }

    bool TinkoffMarket::order(Order order) {
        if (order.side == OrderSide::RESET) {
            if (balance.assetB == 0) {
                return true;
            }

            performOrder(
                abs(balance.assetB) / instrument["lot"].asInt(),
                balance.assetB > 0 ? OrderDirection::ORDER_DIRECTION_SELL : OrderDirection::ORDER_DIRECTION_BUY
            );
        } else if (order.side == OrderSide::BUY) {
            performOrder(1, OrderDirection::ORDER_DIRECTION_BUY);
        } else if (order.side == OrderSide::SELL) {
            performOrder(1, OrderDirection::ORDER_DIRECTION_SELL);
        }
        balance = getBalance();
        lastOrder = order;
        updateBalance();
        return true;
    }

    bool TinkoffMarket::update() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        bool result = LoadCandlesHistory();
        if (result) {
            std::cerr << getBalance().asAssetA() << std::endl;
        }
        return result;
    }

    Helpers::VectorView<Candle> TinkoffMarket::getCandles() const {
        return candles;
    }

    double TinkoffMarket::getCurrency(const Json::Value& positions, const std::string& assetKey, const std::string& asset) {
        for (const Json::Value& position : positions) {
            if (assetKey == "balance") {
                return std::atoi(position[assetKey].asString().c_str());
            }
            if (position[assetKey] == asset) {
                return doubleFromTinkoffFormat(position);
            }
        }
        return 0;
    }

    void TinkoffMarket::updateBalance() {
        Json::Value positions = tinkoffClient.OperationsService_GetPositions(accountId);
        balance = {
            .assetA = getCurrency(positions["money"], "currency", assetA),
            .assetB = getCurrency(positions["securities"], "balance", assetB)
        };
    }

    Balance TinkoffMarket::getBalance() const {
        return balance;
    }

    double TinkoffMarket::getFee() const {
        return fee;
    }

} // namespace TradingBot
