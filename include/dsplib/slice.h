#pragma once

#include <algorithm>
#include <cstring>

#include <dsplib/assert.h>
#include <dsplib/iterator.h>

namespace dsplib {

template<typename T>
class base_array;

template<typename T>
class slice_t;

template<typename T>
class const_slice_t;

//----------------------------------------------------------------------------------------
class base_slice_t
{
public:
    explicit base_slice_t(int n, int i1, int i2, int m) {
        DSPLIB_ASSERT(n != 0, "Slicing from an empty array");
        DSPLIB_ASSERT(m != 0, "Slice stride cannot be zero");

        _m = m;
        _n = n;
        _i1 = (i1 < 0) ? (_n + i1) : (i1);
        _i2 = (i2 < 0) ? (_n + i2) : (i2);
        const int d = std::abs(_i2 - _i1);
        const int tm = std::abs(_m);
        _nc = (d % tm != 0) ? (d / tm + 1) : (d / tm);

        if ((_i1 < 0) || (_i1 >= _n)) {
            DSPLIB_THROW("Left slice index out of range");
        }

        if ((_i2 < 0) || (_i2 > _n)) {
            DSPLIB_THROW("Right slice index out of range");
        }

        if ((_m < 0) && (_i1 < _i2)) {
            DSPLIB_THROW("First index is smaller for negative step");
        }

        if ((_m > 0) && (_i1 > _i2)) {
            DSPLIB_THROW("First index is greater for positive step");
        }

        if (_nc > _n) {
            DSPLIB_THROW("Slice range is greater vector size");
        }
    }

protected:
    int _i1{0};
    int _i2{0};
    int _m{0};
    int _n{0};
    int _nc{0};
};

//----------------------------------------------------------------------------------------
//TODO: add concatenate array = slice | array
template<typename T>
class const_slice_t : public base_slice_t
{
public:
    friend class base_array<T>;
    friend class slice_t<T>;
    using const_iterator = SliceIterator<const T>;

    const_slice_t(const base_array<T>& arr, int i1, int i2, int m)
      : base_slice_t(arr.size(), i1, i2, m)
      , _base{arr} {
    }

    const_slice_t(const const_slice_t& rhs)
      : base_slice_t(rhs.size(), rhs._i1, rhs._i2, rhs._m)
      , _base{rhs._base} {
    }

    const_slice_t(const slice_t<T>& rhs)
      : base_slice_t(rhs._n, rhs._i1, rhs._i2, rhs._m)
      , _base{rhs._base} {
    }

    [[nodiscard]] int size() const noexcept {
        return _nc;
    }

    [[nodiscard]] int stride() const noexcept {
        return _m;
    }

    const_iterator begin() const noexcept {
        return const_iterator(_base.data() + _i1, _m);
    }

    const_iterator end() const noexcept {
        auto current = begin();
        std::advance(current, _nc);
        return current;
    }

    base_array<T> operator*() const noexcept {
        return base_array<T>(*this);
    }

private:
    const base_array<T>& _base;
};

//----------------------------------------------------------------------------------------
template<typename T>
class slice_t : public base_slice_t
{
public:
    friend class base_array<T>;
    friend class const_slice_t<T>;
    using iterator = SliceIterator<T>;
    using const_iterator = SliceIterator<const T>;

    slice_t(base_array<T>& arr, int i1, int i2, int m)
      : base_slice_t(arr.size(), i1, i2, m)
      , _base{arr} {
    }

    slice_t(const slice_t& rhs)
      : base_slice_t(rhs._n, rhs._i1, rhs._i2, rhs._m)
      , _base{rhs._base} {
    }

    [[nodiscard]] int size() const noexcept {
        return _nc;
    }

    [[nodiscard]] int stride() const noexcept {
        return _m;
    }

    slice_t& operator=(const const_slice_t<T>& rhs) {
        DSPLIB_ASSERT(this->size() == rhs.size(), "Slices size must be equal");
        const int count = this->size();

        //empty slice assign
        if (count == 0) {
            return *this;
        }

        //simple block copy/move (optimization)
        const bool is_same_vec = (_base.data() == rhs._base.data());
        if ((this->stride() == 1) && (rhs.stride() == 1)) {
            const auto* src = &(*rhs.begin());
            auto* dst = &(*begin());
            if (!is_same_vec) {
                std::memcpy(dst, src, count * sizeof(*src));
            } else {
                //TODO: check overlap and use memcpy
                std::memmove(dst, src, count * sizeof(*src));
            }
            return *this;
        }

        //same array, specific indexing
        if (is_same_vec) {
            *this = base_array<T>(rhs);
            return *this;
        }

        std::copy(rhs.begin(), rhs.end(), this->begin());
        return *this;
    }

    slice_t& operator=(const slice_t<T>& rhs) {
        *this = const_slice_t<T>(rhs);
        return *this;
    }

    slice_t& operator=(const base_array<T>& rhs) {
        DSPLIB_ASSERT(&_base != &rhs, "Assigned array to same slice");
        return (*this = rhs.slice(0, rhs.size()));
    }

    slice_t& operator=(const T& value) {
        std::fill(begin(), end(), value);
        return *this;
    }

    slice_t& operator=(const std::initializer_list<T>& rhs) {
        std::copy(rhs.begin(), rhs.end(), this->begin());
        return *this;
    }

    iterator begin() noexcept {
        return iterator(_base.data() + _i1, _m);
    }

    iterator end() noexcept {
        auto current = begin();
        std::advance(current, _nc);
        return current;
    }

    const_iterator begin() const noexcept {
        return const_iterator(_base.data() + _i1, _m);
    }

    const_iterator end() const noexcept {
        auto current = begin();
        std::advance(current, _nc);
        return current;
    }

    base_array<T> operator*() const noexcept {
        return base_array<T>(*this);
    }

private:
    base_array<T>& _base;
};

}   // namespace dsplib