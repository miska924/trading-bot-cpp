#include <iostream>

#include "markets/market.h"


int main() {
    trading_bot::Market market = trading_bot::Market();
    std::cout << "Market time: " << market.time() << std::endl;
    return 0;
}
