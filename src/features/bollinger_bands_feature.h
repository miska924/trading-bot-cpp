#pragma once

#include <vector>
#include <cmath>

#include "features/feature.h"
#include "features/sma_feature.h"  // Подключаем SMA
#include "helpers/vector_view.h"
#include "helpers/function_queue.h"


namespace TradingBot {

    const int DEFAULT_BB_PERIOD = 20;
    const double DEFAULT_BB_K = 2.0;

    class BollingerBandsFeature : public Feature {
    public:
        BollingerBandsFeature(int period = DEFAULT_BB_PERIOD, double k = DEFAULT_BB_K);
        int getPeriod() const;
        virtual double operator()(const Helpers::VectorView<Candle>& candles, bool incremental = false) override;
        
    private:
        int period;
        double k;  // множитель стандартного отклонения
        SMAFeature smaFeature;  // Экземпляр для вычисления SMA
        Helpers::FunctionQueue<double> queue;

        double calculateSTD(const Helpers::VectorView<Candle>& candles, double sma);
    };

} // namespace TradingBot