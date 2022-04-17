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
        const int n1 = (_i2 - _i1) / _m;
        const int n2 = (rhs._i2 - rhs._i1) / rhs._m;
        if (n1 != n2) {
            throw std::out_of_range("Not equal size");
        }

        if (n1 == 0 && n2 == 0) {
            return *this;
        }

        int i1 = (_i1 + _n) % _n;
        int i2 = (rhs._i1 + rhs._n) % rhs._n;
        T* data = _base.data();
        const T* rdata = rhs._base.data();
        for (size_t i = 0; i < n2; i++) {
            data[i1] = rdata[i2];
            i1 = (i1 + _m + _n) % _n;
            i2 = (i2 + rhs._m + rhs._n) % rhs._n;
        }

        return *this;
    }

    slice_t& operator=(const slice_t<T>& rhs) {
        *this = const_slice_t<T>(rhs);
        return *this;
    }

    slice_t& operator=(const base_array<T>& rhs) {
        const int n1 = (_i2 - _i1) / _m;
        const int n2 = rhs.size();
        if (n1 != n2) {
            throw std::out_of_range("Not equal size");
        }

        if (n1 == 0 && n2 == 0) {
            return *this;
        }

        int i1 = (_i1 + _n) % _n;
        T* data = _base.data();
        for (size_t i = 0; i < n2; i++) {
            data[i1] = rhs[i];
            i1 = (i1 + _m + _n) % _n;
        }

        return *this;
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
        const int n1 = (_i2 - _i1) / _m;
        const int n2 = list.size();
        if (n1 != n2) {
            throw std::out_of_range("Not equal size");
        }

        if (n1 == 0 && n2 == 0) {
            return *this;
        }

        int i1 = (_i1 + _n) % _n;
        const auto* d2 = list.begin();
        T* data = _base.data();
        for (size_t i = 0; i < n2; i++) {
            data[i1] = d2[i];
            i1 = (i1 + _m + _n) % _n;
        }

        return *this;
    }

private:
    int _i1{0};
    int _i2{0};
    int _m{0};
    int _n{0};
    base_array<T>& _base;
};

}   // namespace dsplib