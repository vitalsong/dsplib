#pragma once

#include <cstring>

#include <dsplib/throw.h>

namespace dsplib {

template<typename T>
class base_array;

class base_slice_t;

template<typename T>
class slice_t;

template<typename T>
class const_slice_t;

class base_slice_t
{
public:
    explicit base_slice_t(int n, int i1, int i2, int m) {
        if (n == 0) {
            DSPLIB_THROW("Slicing from an empty array");
        }

        if (m == 0) {
            DSPLIB_THROW("Slice stride cannot be zero");
        }

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

template<typename T>
class const_slice_t : public base_slice_t
{
public:
    friend class base_array<T>;
    friend class slice_t<T>;

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

    int size() const {
        return _nc;
    }

private:
    const base_array<T>& _base;
};

template<typename T>
class slice_t : public base_slice_t
{
public:
    friend class base_array<T>;
    friend class const_slice_t<T>;

    slice_t(base_array<T>& arr, int i1, int i2, int m)
      : base_slice_t(arr.size(), i1, i2, m)
      , _base{arr} {
    }

    slice_t(const slice_t& rhs)
      : base_slice_t(rhs._n, rhs._i1, rhs._i2, rhs._m)
      , _base{rhs._base} {
    }

    int size() const {
        return _nc;
    }

    slice_t& operator=(const const_slice_t<T>& rhs) {
        const int dst_step = _m;
        const int src_step = rhs._m;
        const int src_t0 = rhs._i1;
        const int src_t1 = rhs._i2;
        const int dst_t0 = _i1;
        const int dst_t1 = _i2;
        const int dst_count = _nc;
        const int src_count = rhs._nc;

        const int count = dst_count;
        if (dst_count != src_count) {
            DSPLIB_THROW("Not equal size");
        }

        if (count == 0) {
            return *this;
        }

        T* dst_p = _base.data();
        const T* src_p = rhs._base.data();
        const bool is_same_vec = (dst_p == src_p);
        const bool is_dirrect = (dst_t0 < dst_t1) && (src_t0 < src_t1);

        //simple block copy/move
        if ((dst_step == src_step) && (dst_step == 1) && is_dirrect) {
            if (!is_same_vec) {
                std::memcpy((dst_p + dst_t0), (src_p + src_t0), count * sizeof(T));
            } else {
                std::memmove((dst_p + dst_t0), (src_p + src_t0), count * sizeof(T));
            }
            return *this;
        }

        if (!is_same_vec) {
            _assign(src_p, dst_p, src_step, dst_step, src_t0, dst_t0, count);
        } else {
            base_array<T> tmp(_base);
            _assign(tmp.data(), dst_p, src_step, dst_step, src_t0, dst_t0, count);
        }

        return *this;
    }

    slice_t& operator=(const slice_t<T>& rhs) {
        *this = const_slice_t<T>(rhs);
        return *this;
    }

    slice_t& operator=(const base_array<T>& rhs) {
        if (&_base == &rhs) {
            DSPLIB_THROW("Assigned array to same slice");
        }
        return (*this = rhs.slice(0, rhs.size()));
    }

    slice_t& operator=(const T& value) {
        const int n = (_i2 - _i1) / _m;
        int i1 = (_i1 + _n) % _n;
        T* data = _base.data();
        for (size_t i = 0; i < n; i++) {
            data[i1] = value;
            i1 = (i1 + _m + _n) % _n;
        }

        return *this;
    }

    slice_t& operator=(const std::initializer_list<T>& list) {
        return (*this = base_array<T>(list));
    }

private:
    static void _assign(const T* restrict src, T* restrict dst, int src_step, int dst_step, int src_init, int dst_init,
                        int count) {
        int idx_dst = dst_init;
        int idx_src = src_init;
        for (size_t i = 0; i < count; i++) {
            dst[idx_dst] = src[idx_src];
            idx_dst += dst_step;
            idx_src += src_step;
        }
    }

    base_array<T>& _base;
};

}   // namespace dsplib