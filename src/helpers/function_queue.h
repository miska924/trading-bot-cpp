#pragma once

#include <vector>
#include <functional>


namespace Helpers {

    template <typename T>
    class FunctionQueue {
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

        T functionValue() const {
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

} // namespace Helpers
