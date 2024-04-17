#include "markets/tinkoff/tinkoff_api.h"

#include <cstdlib>


const std::string TINKOFF_TOKEN = std::getenv("TINKOFF_TOKEN");
const std::string TINKOFF_SB_TOKEN = std::getenv("TINKOFF_SB_TOKEN");

namespace Tinkoff {

    std::string getApiToken(bool sandbox) {
        return sandbox ? TINKOFF_SB_TOKEN : TINKOFF_TOKEN;
    }
    
} // namespace Tinkoff
