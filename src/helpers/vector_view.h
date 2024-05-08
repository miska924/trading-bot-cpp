#pragma once

#include <assert.h>


namespace Helpers {

    template <class T>
    class VectorView {
    public:
        VectorView() = delete;
        VectorView(const std::vector<T>& vector): _vector(vector) {
            _begin = 0;
            _end = vector.size();
            _size = vector.size();
        }

        VectorView(const std::vector<T>& vector, size_t begin, size_t end): _vector(vector) {
            assert(begin <= end && end <= vector.size());
            _begin = begin;
            _end = end;
            _size = end - begin;
        }

        VectorView(
            const VectorView<T>& other
        ): _vector(other._vector), _begin(other._begin), _end(other._end), _size(other._size) {
        }

        size_t size() const {
            return _size;
        }

        const T& operator[](size_t i) const {
            size_t index = _begin + i;
            assert(index < _end);
            return _vector[index];
        }

        VectorView<T> subView(size_t begin, size_t end) const {
            assert(begin <= end && end <= _size);
            return VectorView<T>(_vector, this->_begin + begin, this->_begin + end);
        }

        std::vector<T> toVector() const {
            return std::vector<T>(_vector.begin() + _begin, _vector.begin() + _end);
        }

        const T& back() const {
            assert(_begin < _end);
            return _vector[_end - 1];
        }

    private:
        const std::vector<T>& _vector;
        size_t _begin;
        size_t _end;
        size_t _size;
    };

} // namespace Helpers
