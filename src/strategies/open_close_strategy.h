#include "strategies/strategy.h"


namespace TradingBot {

    template <typename OpenStrat, typename CloseStrat>
    class OpenCloseStrategy : public Strategy {
    public:
        OpenCloseStrategy(Market* _market, const ParamSet& paramSet);
        virtual void step() override;
        virtual bool checkParamSet(const ParamSet& paramSet) const override;
    protected:
        virtual void openStep() = 0;
        virtual void closeStep() = 0;
    private:
        bool checkThisParamSet() const;
        ParamSet openParamSet;
        ParamSet closeParamSet;
    };


    template <typename OpenStrat, typename CloseStrat>
    OpenCloseStrategy<OpenStrat, CloseStrat>::OpenCloseStrategy(Market* market, const ParamSet& paramSet) {
        size_t openParamSetSize = OpenStrat::getParamSetSize();
        size_t closeParamSetSize = CloseStrat::getParamSetSize();
        openParamSet = ParamSet(paramSet.begin(), paramSet.begin() + openParamSetSize);
        closeParamSet = ParamSet(paramSet.begin() + openParamSetSize, paramSet.end());
        assert(checkThisParamSet());

        this->paramSet = paramSet;
        this->market = market;
    }


    template <typename OpenStrat, typename CloseStrat>
    void OpenCloseStrategy<OpenStrat, CloseStrat>::step() {
        openStep();
        closeStep();
    }


    template <typename OpenStrat, typename CloseStrat>
    bool OpenCloseStrategy<OpenStrat, CloseStrat>::checkThisParamSet() const {
        return OpenStrat::checkParamSet(openParamSet) && CloseStrat::checkParamSet(closeParamSet);
    }


    template <typename OpenStrat, typename CloseStrat>
    bool OpenCloseStrategy<OpenStrat, CloseStrat>::checkParamSet(const ParamSet& paramSet) const {
        size_t openParamSetSize = OpenStrat::getParamSetSize();
        size_t closeParamSetSize = CloseStrat::getParamSetSize();
        ParamSet openParamSet = ParamSet(paramSet.begin(), paramSet.begin() + openParamSetSize);
        ParamSet closeParamSet = ParamSet(paramSet.begin() + openParamSetSize, paramSet.end());
        return OpenStrat::checkParamSet(openParamSet) && CloseStrat::checkParamSet(closeParamSet);
    }


    template <typename OpenStrat, typename CloseStrat>
    void OpenCloseStrategy<OpenStrat, CloseStrat>::openStep() {
        OpenStrat openStrat(market, openParamSet);
        openStrat.run();
    }

} // namespace TradingBot
