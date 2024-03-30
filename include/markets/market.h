#pragma once

#include <time.h>


namespace trading_bot {

    class Market {
    public:
        virtual ~Market() = default;
        virtual time_t time() const;
    };

}
