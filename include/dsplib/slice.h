#pragma once

namespace dsplib {

template<typename T>
class base_array;

template<typename T>
class slice_t;

template<typename T>
class const_slice_t;

//TODO: use common slice class
template<typename T>
class const_slice_t
{
public:
    friend class base_array<T>;
    friend class slice_t<T>;

    const_slice_t(const base_array<T>& arr, int i1, int i2, int m)
      : _i1{i1}
      , _i2{i2}
      , _m{m}
      , _n{arr.size()}
      , _base{arr} {
        if (_m < 1) {
            throw std::range_error("Step must be greater 1");
        }

        if (_i2 - _i1 > _n) {
            throw std::range_error("Slice range is greater vector size");
        }
    }

    const_slice_t(const const_slice_t& rhs) {
        *this = rhs;
    }

    const_slice_t(const slice_t<T>& rhs)
      : _i1{rhs._i1}
      , _i2{rhs._i2}
      , _m{rhs._m}
      , _n{rhs._n}
      , _base{rhs._base} {
    }

private:
    int _i1{0};
    int _i2{0};
    int _m{0};
    int _n{0};
    const base_array<T>& _base;
};

template<typename T>
class slice_t
{
public:
    friend class base_array<T>;
    friend class const_slice_t<T>;

    slice_t(base_array<T>& arr, int i1, int i2, int m)
      : _i1{i1}
      , _i2{i2}
      , _m{m}
      , _n{arr.size()}
      , _base{arr} {
        if (_m < 1) {
            throw std::range_error("Step must be greater 1");
        }

        if (_i2 - _i1 > _n) {
            throw std::range_error("Slice range is greater vector size");
        }
    }

    slice_t(const slice_t& rhs)
      : _i1{rhs._i1}
      , _i2{rhs._i2}
      , _m{rhs._m}
      , _n{rhs._n}
      , _base{rhs._base} {
    }

    slice_t& operator=(const const_slice_t<T>& rhs) {
        const int dst_step = _m;
        const int src_step = rhs._m;
        const int src_t0 = rhs._i1;
        const int src_t1 = rhs._i2;
        const int dst_t0 = _i1;
        const int dst_t1 = _i2;
        const int dst_count = (dst_t1 - dst_t0) / dst_step;
        const int src_count = (src_t1 - src_t0) / src_step;
        const int dst_n = _n;
        const int src_n = rhs._n;

        const int count = dst_count;
        if (dst_count != src_count) {
            throw std::out_of_range("Not equal size");
        }

        if (count == 0) {
            return *this;
        }

        T* dst_p = _base.data();
        const T* src_p = rhs._base.data();
        const bool is_same_vec = (dst_p == src_p);
        const bool not_ciclic = (dst_t0 >= 0) && (src_t0 >= 0) && (dst_t1 <= dst_n) && (src_t1 <= src_n);

        //simple block copy/move
        if ((dst_step == src_step) && (dst_step == 1) && not_ciclic) {
            if (!is_same_vec) {
                memcpy((dst_p + dst_t0), (src_p + src_t0), count * sizeof(T));
            } else {
                memmove((dst_p + dst_t0), (src_p + src_t0), count * sizeof(T));
            }
            return *this;
        }

        if (!is_same_vec) {
            _assign(src_p, dst_p, src_step, dst_step, src_n, dst_n, src_t0, dst_t0, count);
        } else {
            base_array<T> tmp(_base);
            _assign(tmp.data(), dst_p, src_step, dst_step, src_n, dst_n, src_t0, dst_t0, count);
        }

        return *this;
    }

    slice_t& operator=(const slice_t<T>& rhs) {
        *this = const_slice_t<T>(rhs);
        return *this;
    }

    slice_t& operator=(const base_array<T>& rhs) {
        if (&_base == &rhs) {
            throw std::runtime_error("Assigned array to same slice");
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
    static void _assign(const T* restrict src, T* restrict dst, int src_step, int dst_step, int src_n, int dst_n,
                        int src_init, int dst_init, int count) {
        int idx_dst = (dst_init + dst_n) % dst_n;
        int idx_src = (src_init + src_n) % src_n;
        for (size_t i = 0; i < count; i++) {
            dst[idx_dst] = src[idx_src];
            idx_dst = (idx_dst + dst_step + dst_n) % dst_n;
            idx_src = (idx_src + src_step + src_n) % src_n;
        }
    }

    int _i1{0};
    int _i2{0};
    int _m{0};
    int _n{0};
    base_array<T>& _base;
};

}   // namespace dsplib