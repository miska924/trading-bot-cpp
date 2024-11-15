#pragma once

#include <vector>
#include <functional>


namespace Helpers {

    template <typename T>
    class IFunctionQueue {
    public:
        virtual void push(const T& value) = 0;
        virtual T pop() = 0;
        virtual T getValue() const = 0;
        virtual size_t size() const = 0;
        virtual void clear() = 0;
    };

    template <typename T>
    class FunctionQueue: IFunctionQueue<T> {
    public:
        FunctionQueue(std::function<T(const T&, const T&)> func) : f(func) {}

        void push(const T& value) {
            pushWithFunc(head, headFunc, value);
        }

        T pop() {
            if (tail.empty()) {
                moveHeadToTail();
            }
            return popWithFunc(tail, tailFunc);
        }

        T getValue() const {
            if (headFunc.size() == 0) {
                return tailFunc.back();
            }
            if (tailFunc.size() == 0) {
                return headFunc.back();
            }
            return f(headFunc.back(), tailFunc.back());
        }

        size_t size() const {
            return head.size() + tail.size();
        }

        void clear() {
            head.clear();
            tail.clear();
            headFunc.clear();
            tailFunc.clear();
        }

    private:

        void pushWithFunc(std::vector<T>& v, std::vector<T>& func, const T& value) {
            v.push_back(value);
            if (func.size() == 0) {
                func.push_back(value);
                return;
            }
            func.push_back(f(func.back(), value));
        }

        T popWithFunc(std::vector<T>& v, std::vector<T>& func) {
            T result = v.back();
            v.pop_back();
            func.pop_back();
            return result;
        }

        void moveHeadToTail() {
            while (!head.empty()) {
                pushWithFunc(
                    tail,
                    tailFunc,
                    popWithFunc(head, headFunc)
                );
            }
        }

        std::vector<T> head;
        std::vector<T> headFunc;
        std::vector<T> tail;
        std::vector<T> tailFunc;

        std::function<T(const T&, const T&)> f;

    };

    template <typename T>
    class WeightedSumQueue: IFunctionQueue<T> {
    public:
        WeightedSumQueue(T k) : k(k) {
            savedPowerK.push_back(static_cast<T>(1));
        }

        void push(const T& value) {
            head.push_back(value);
            headFunc.push_back(
                value + (headFunc.empty()? static_cast<T>(0) : headFunc.back() * k)
            );
        }

        T pop() {
            if (tail.empty()) {
                moveHeadToTail();
            }
            return popWithFunc(tail, tailFunc);
        }

        T getValue() const {
            if (headFunc.size() == 0) {
                return tailFunc.back();
            }
            if (tailFunc.size() == 0) {
                return headFunc.back();
            }
            return headFunc.back() + tailFunc.back() * powerK(headFunc.size());
        }

        size_t size() const {
            return head.size() + tail.size();
        }

        void clear() {
            head.clear();
            tail.clear();
            headFunc.clear();
            tailFunc.clear();
        }

        T powerK(size_t p) const {
            if (p < savedPowerK.size()) {
                return savedPowerK[p];
            }
            savedPowerK.reserve(p + 1);
            for (size_t power = savedPowerK.size(); power <= p; ++power) {
                savedPowerK.push_back(savedPowerK.back() * k);
            }
            return savedPowerK[p];
        }

    private:
        void pushTail(std::vector<T>& v, std::vector<T>& func, const T& value) {
            v.push_back(value);
            if (func.size() == 0) {
                func.push_back(value);
                return;
            }
            func.push_back(func.back() + value * powerK(func.size()));
        }

        T popWithFunc(std::vector<T>& v, std::vector<T>& func) {
            T result = v.back();
            v.pop_back();
            func.pop_back();
            return result;
        }

        void moveHeadToTail() {
            while (!head.empty()) {
                pushTail(
                    tail,
                    tailFunc,
                    popWithFunc(head, headFunc)
                );
            }
        }

        std::vector<T> head;
        std::vector<T> headFunc;
        std::vector<T> tail;
        std::vector<T> tailFunc;

        T k;
        mutable std::vector<T> savedPowerK;
    };

} // namespace Helpers
