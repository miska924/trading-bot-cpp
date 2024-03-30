#include "markets/market.h"

#include <chrono>
#include <time.h>


namespace trading_bot {

time_t Market::time() const {
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

} // trading_bot
