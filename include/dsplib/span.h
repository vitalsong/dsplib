#pragma once

#include <dsplib/slice.h>

namespace dsplib {

template<class T>
class const_span_t;

template<class T>
class span_t;

//span is a slice with a stride of 1
//used to quickly access vector elements in functions (without memory allocation)

//mutable span
template<typename T>
class span_t : public slice_t<T>
{
public:
    friend class const_span_t<T>;

    explicit span_t(base_array<T>& v, int i1, int i2)
      : slice_t<T>(v, i1, i2, 1)
      , _ptr{v.data() + i1}
      , _len{slice_t<T>::size()} {
    }

    span_t(base_array<T>& v)
      : span_t(v, 0, v.size()) {
    }

    T* data() noexcept {
        return _ptr;
    }

    int size() const noexcept {
        return _len;
    }

    T& operator()(int i) noexcept {
        assert((i >= 0) && (i < _len));
        return _ptr[i];
    }

    T& operator[](int i) noexcept {
        assert((i >= 0) && (i < _len));
        return _ptr[i];
    }

    //TODO: override fast implementation for span

    span_t& operator=(const const_slice_t<T>& rhs) {
        slice_t<T>::operator=(rhs);
        return *this;
    }

    span_t& operator=(const slice_t<T>& rhs) {
        slice_t<T>::operator=(rhs);
        return *this;
    }

    span_t& operator=(const base_array<T>& rhs) {
        slice_t<T>::operator=(rhs);
        return *this;
    }

    span_t& operator=(const T& rhs) {
        std::fill(_ptr, (_ptr + _len), rhs);
        return *this;
    }

    span_t& operator=(const std::initializer_list<T>& rhs) {
        slice_t<T>::operator=(rhs);
        return *this;
    }

private:
    T* _ptr{nullptr};
    int _len{0};
};

//immutable span
template<typename T>
class const_span_t : public const_slice_t<T>
{
public:
    friend class span_t<T>;

    explicit const_span_t(const base_array<T>& v, int i1, int i2)
      : const_slice_t<T>(v, i1, i2, 1)
      , _ptr{v.data() + i1}
      , _len{const_slice_t<T>::size()} {
    }

    const_span_t(const span_t<T>& v)
      : const_span_t(v._base, v._i1, v._i2) {
    }

    const_span_t(const base_array<T>& v)
      : const_span_t(v, 0, v.size()) {
    }

    const T* data() const noexcept {
        return _ptr;
    }

    int size() const noexcept {
        return _len;
    }

    const T& operator()(int i) const noexcept {
        assert((i >= 0) && (i < _len));
        return _ptr[i];
    }

    const T& operator[](int i) const noexcept {
        assert((i >= 0) && (i < _len));
        return _ptr[i];
    }

private:
    const T* _ptr{nullptr};
    int _len{0};
};

using const_span_real = const_span_t<real_t>;
using const_span_cmplx = const_span_t<cmplx_t>;

}   // namespace dsplib