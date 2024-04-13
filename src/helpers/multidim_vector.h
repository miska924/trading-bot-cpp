#pragma once

#include <assert.h>
#include <vector>


namespace Helpers {

    template <class T>
    class MultidimVector {
    public:
        MultidimVector() = default;
        MultidimVector(const std::vector<size_t>& shape, const T& value = T()) : shape(shape) {
            int size = 1;
            for (int i = 0; i < shape.size(); ++i) {
                size *= shape[i];
            }
            data.resize(size, value);
        }

        T& operator[](const std::vector<size_t>& index) {
            int i = 0;
            for (int j = 0; j < index.size(); ++j) {
                assert(index[j] < shape[j]);
                i *= shape[j];
                i += index[j];
            }
            return data[i];
        }

        const T& operator[](const std::vector<size_t>& index) const {
            int i = 0;
            for (int j = 0; j < index.size(); ++j) {
                assert(index[j] < shape[j]);
                i *= shape[j];
                i += index[j];
            }
            return data[i];
        }

        T& operator[](size_t i) {
            assert(i < data.size());
            return data[i];
        }

        const T& operator[](size_t i) const {
            assert(i < data.size());
            return data[i];
        }

        const std::vector<size_t>& getShape() const {
            return shape;
        }

        size_t size() const {
            return data.size();
        }

        std::vector<size_t> getIndex(size_t i) const {
            assert(i < data.size());
            std::vector<size_t> index(shape.size());
            for (int j = shape.size() - 1; j >= 0; --j) {
                index[j] = i % shape[j];
                i /= shape[j];
            }
            return index;
        }

    private:
        std::vector<T> data;
        std::vector<size_t> shape;
    };

} // namespace Helpers
