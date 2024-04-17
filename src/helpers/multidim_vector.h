#pragma once

#include <vector>

namespace TradingBot {

    template <class T>
    class MultidimVector {
    public:
        MultidimVector(std::vector<int> shape) : shape(shape) {
            int size = 1;
            for (int i = 0; i < shape.size(); ++i) {
                size *= shape[i];
            }
            data.resize(size);
        }

        T& operator[](const vector<int>& index) {
            int i = 0;
            for (int j = 0; j < index.size(); ++j) {
                i *= shape[j];
                i += index[j];
            }
            return data[i];
        }

        const T& operator[](const vector<int>& index) const {
            int i = 0;
            for (int j = 0; j < index.size(); ++j) {
                i *= shape[j];
                i += index[j];
            }
            return data[i];
        }

        T& operator[](int i) {
            return data[i];
        }

        const T& operator[](int i) const {
            return data[i];
        }

        const std::vector<int>& getShape() const {
            return shape;
        }

        int size() const {
            return data.size();
        }

    private:
        std::vector<T> data;
        std::vector<int> shape;
    };
}