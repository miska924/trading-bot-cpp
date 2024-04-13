#pragma once

#include <assert.h>


namespace Helpers {

    template <class T>
    class VectorView {
    public:
        VectorView() = delete;
        VectorView(const std::vector<T>& vector) {
            begin = vector.begin();
            end = vector.end();
        }

        VectorView(
            std::vector<T>::const_iterator begin,
            std::vector<T>::const_iterator end
        ): begin(begin), end(end) {}

        VectorView(
            const VectorView<T>& other
        ): begin(other.begin), end(other.end) {}

        int size() const {
            return end - begin;
        }

        const T& operator[](int i) const {
            assert(0 <= i && i < size());
            return (*(begin + i));
        }

        VectorView<T> subView(int begin, int end) const {
            assert(0 <= begin && begin <= end && end <= size());
            return VectorView<T>(this->begin + begin, this->begin + end);
        }

        std::vector<T> toVector() const {
            return std::vector<T>(begin, end);
        }

    private:
        std::vector<T>::const_iterator begin;
        std::vector<T>::const_iterator end;
    };

} // namespace Helpers
