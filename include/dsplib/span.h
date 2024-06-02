#pragma once

#include <dsplib/throw.h>
#include <dsplib/types.h>
#include <dsplib/slice.h>

#include <cassert>
#include <vector>

namespace dsplib {

template<class T>
class mut_span_t;

template<class T>
class span_t;

//span is a slice with a stride of 1
//used to quickly access vector elements in functions (without memory allocation)

//TODO: add concatenate syntax
//TODO: add math operators (+,-,*,/)

//mutable span
template<typename T>
class mut_span_t : public slice_t<T>
{
public:
    friend class span_t<T>;

    explicit mut_span_t(T* data, int size)
      : slice_t<T>(data, size, 0, size, 1)
      , _ptr{data}
      , _len{size} {
    }

    mut_span_t(base_array<T>& v)
      : mut_span_t(v.data(), v.size()) {
    }

    mut_span_t(std::vector<T>& v)
      : mut_span_t(v.data(), v.size()) {
    }

    [[nodiscard]] T* data() noexcept {
        return _ptr;
    }

    [[nodiscard]] const T* data() const noexcept {
        return _ptr;
    }

    [[nodiscard]] int size() const noexcept {
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

    mut_span_t& operator=(const const_slice_t<T>& rhs) {
        slice_t<T>::operator=(rhs);
        return *this;
    }

    mut_span_t& operator=(const slice_t<T>& rhs) {
        slice_t<T>::operator=(rhs);
        return *this;
    }

    mut_span_t& operator=(const base_array<T>& rhs) {
        slice_t<T>::operator=(rhs);
        return *this;
    }

    mut_span_t& operator=(const T& rhs) {
        std::fill(_ptr, (_ptr + _len), rhs);
        return *this;
    }

    mut_span_t& operator=(const std::initializer_list<T>& rhs) {
        slice_t<T>::operator=(rhs);
        return *this;
    }

    using iterator = T*;
    using const_iterator = const T*;

    iterator begin() noexcept {
        return _ptr;
    }

    iterator end() noexcept {
        return _ptr + _len;
    }

    const_iterator begin() const noexcept {
        return _ptr;
    }

    const_iterator end() const noexcept {
        return _ptr + _len;
    }

    //TODO: add more checks
    mut_span_t slice(int i1, int i2) const {
        DSPLIB_ASSERT((i1 >= 0) && (i1 < _len), "invalid range");
        DSPLIB_ASSERT((i2 > i1) && (i2 <= _len), "invalid range");
        return mut_span_t(_ptr + i1, (i2 - i1));
    }

private:
    T* _ptr{nullptr};
    int _len{0};
};

//immutable span
template<typename T>
class span_t : public const_slice_t<T>
{
public:
    friend class mut_span_t<T>;

    explicit span_t(const T* data, int size)
      : const_slice_t<T>(data, size, 0, size, 1)
      , _ptr{data}
      , _len{size} {
    }

    span_t(const mut_span_t<T>& v)
      : span_t(v.data(), v.size()) {
    }

    span_t(const base_array<T>& v)
      : span_t(v.data(), v.size()) {
    }

    span_t(const std::vector<T>& v)
      : span_t(v.data(), v.size()) {
    }

    [[nodiscard]] const T* data() const noexcept {
        return _ptr;
    }

    [[nodiscard]] int size() const noexcept {
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

    using const_iterator = const T*;

    const_iterator begin() const noexcept {
        return _ptr;
    }

    const_iterator end() const noexcept {
        return _ptr + _len;
    }

    span_t slice(int i1, int i2) const {
        DSPLIB_ASSERT((i1 >= 0) && (i1 < _len), "invalid range");
        DSPLIB_ASSERT((i2 > i1) && (i2 <= _len), "invalid range");
        return span_t(_ptr + i1, (i2 - i1));
    }

private:
    const T* _ptr{nullptr};
    int _len{0};
};

//------------------------------------------------------------------------------------------------
//TODO: rename to `make_span`?
template<typename T>
span_t<T> span(const T* x, int nx) noexcept {
    return span_t<T>(x, nx);
}

template<typename T>
mut_span_t<T> span(T* x, int nx) noexcept {
    return mut_span_t<T>(x, nx);
}

//------------------------------------------------------------------------------------------------
template<typename T>
span_t<T> span(const std::vector<T>& x) noexcept {
    return span_t<T>(x.data(), x.size());
}

template<typename T>
mut_span_t<T> span(std::vector<T>& x) noexcept {
    return mut_span_t<T>(x.data(), x.size());
}

//------------------------------------------------------------------------------------------------
template<typename T>
span_t<T> span(const base_array<T>& x) noexcept {
    return span_t<T>(x.data(), x.size());
}

template<typename T>
mut_span_t<T> span(base_array<T>& x) noexcept {
    return mut_span_t<T>(x.data(), x.size());
}

//TODO: short naming, span_r/span_c?
using span_real = span_t<real_t>;
using span_cmplx = span_t<cmplx_t>;

}   // namespace dsplib