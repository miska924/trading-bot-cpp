#pragma once

#include <time.h>

#include "markets/market.h"


namespace trading_bot {

    class BacktestMarket : public Market {
    public:
        time_t time() const override;
        ~BacktestMarket() = default;
    private:
        time_t marketTime = 0;
    };

}
