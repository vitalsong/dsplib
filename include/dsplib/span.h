#pragma once

#include <dsplib/assert.h>
#include <dsplib/types.h>
#include <dsplib/slice.h>
#include <dsplib/traits.h>

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

//mutable span
template<typename T>
class mut_span_t : public mut_slice_t<T>
{
public:
    static_assert(!std::is_same_v<std::remove_cv_t<T>, bool>, "`bool` type is not supported");
    static_assert(std::is_trivially_copyable<T>(), "type must be trivially copyable");

    friend class span_t<T>;

    explicit mut_span_t(T* data, int size)
      : mut_slice_t<T>{data, 1, size} {
    }

    mut_span_t(const mut_span_t& v)
      : mut_span_t(v.data_, v.count_) {
    }

    //disable for another trivially types (uint8_t, int16_t, long double etc)
    template<typename U = T, typename = std::enable_if_t<support_type_for_array<U>()>>
    mut_span_t(base_array<T>& v)
      : mut_span_t(v.data(), v.size()) {
    }

    mut_span_t(std::vector<T>& v)
      : mut_span_t(v.data(), v.size()) {
    }

    [[nodiscard]] T* data() noexcept {
        return this->data_;
    }

    [[nodiscard]] const T* data() const noexcept {
        return this->data_;
    }

    [[nodiscard]] int size() const noexcept {
        return this->count_;
    }

    T& operator[](int i) noexcept {
        assert((i >= 0) && (i < size()));
        return this->data_[i];
    }

    mut_span_t& operator=(const mut_span_t& rhs) {
        if (this == &rhs) {
            return *this;
        }
        this->assign(rhs);
        return *this;
    }

    mut_span_t& operator=(const span_t<T>& rhs) {
        this->assign(rhs);
        return *this;
    }

    //TODO: override fast implementation for span
    mut_span_t& operator=(const slice_t<T>& rhs) {
        mut_slice_t<T>::operator=(rhs);
        return *this;
    }

    mut_span_t& operator=(const mut_slice_t<T>& rhs) {
        mut_slice_t<T>::operator=(rhs);
        return *this;
    }

    template<typename U = T, typename = std::enable_if_t<support_type_for_array<U>()>>
    mut_span_t& operator=(const base_array<T>& rhs) {
        this->assign(rhs);
        return *this;
    }

    mut_span_t& operator=(const T& rhs) {
        std::fill(this->data_, (this->data_ + this->count_), rhs);
        return *this;
    }

    mut_span_t& operator=(const std::initializer_list<T>& rhs) {
        mut_slice_t<T>::operator=(rhs);
        return *this;
    }

    using iterator = T*;
    using const_iterator = const T*;

    iterator begin() noexcept {
        return this->data_;
    }

    iterator end() noexcept {
        return this->data_ + this->count_;
    }

    const_iterator begin() const noexcept {
        return this->data_;
    }

    const_iterator end() const noexcept {
        return this->data_ + this->count_;
    }

    void assign(span_t<T> rhs) {
        DSPLIB_ASSERT(this->size() == rhs.size(), "Span size is not equal");
        if (!is_same_memory(rhs)) {
            std::memcpy(this->data(), rhs.data(), size() * sizeof(T));
        } else {
            std::memmove(this->data(), rhs.data(), size() * sizeof(T));
        }
    }

    //TODO: add more checks
    mut_span_t slice(int i1, int i2) const {
        DSPLIB_ASSERT((i1 >= 0) && (i1 < this->count_), "invalid range");
        DSPLIB_ASSERT((i2 > i1) && (i2 <= this->count_), "invalid range");
        return mut_span_t(this->data_ + i1, (i2 - i1));
    }

    //---------------------------------------------------------------------------
    //arithmetic operators
    template<typename U = T, class T2, typename = std::enable_if_t<support_type_for_array<U>()>>
    mut_span_t& operator+=(const T2& rhs) noexcept(is_scalar_v<T2>) {
        auto* x = data();
        const int n = size();
        using R = ResultType<T, T2>;
        static_assert(std::is_same_v<T, R>, "The operation changes the type");
        if constexpr (is_scalar_v<T2>) {
            for (int i = 0; i < n; ++i) {
                x[i] += rhs;
            }
        } else {
            DSPLIB_ASSERT(n == rhs.size(), "Array lengths must be equal");
            for (int i = 0; i < n; ++i) {
                x[i] += rhs[i];
            }
        }
        return *this;
    }

    template<typename U = T, class T2, typename = std::enable_if_t<support_type_for_array<U>()>>
    mut_span_t& operator-=(const T2& rhs) noexcept(is_scalar_v<T2>) {
        auto* x = data();
        const int n = size();
        using R = ResultType<T, T2>;
        static_assert(std::is_same_v<T, R>, "The operation changes the type");
        if constexpr (is_scalar_v<T2>) {
            for (int i = 0; i < n; ++i) {
                x[i] -= rhs;
            }
        } else {
            DSPLIB_ASSERT(n == rhs.size(), "Array lengths must be equal");
            for (int i = 0; i < n; ++i) {
                x[i] -= rhs[i];
            }
        }
        return *this;
    }

    template<typename U = T, class T2, typename = std::enable_if_t<support_type_for_array<U>()>>
    mut_span_t& operator*=(const T2& rhs) noexcept(is_scalar_v<T2>) {
        auto* x = data();
        const int n = size();
        using R = ResultType<T, T2>;
        static_assert(std::is_same_v<T, R>, "The operation changes the type");
        if constexpr (is_scalar_v<T2>) {
            for (int i = 0; i < n; ++i) {
                x[i] *= rhs;
            }
        } else {
            DSPLIB_ASSERT(n == rhs.size(), "Array lengths must be equal");
            for (int i = 0; i < n; ++i) {
                x[i] *= rhs[i];
            }
        }
        return *this;
    }

    template<typename U = T, class T2, typename = std::enable_if_t<support_type_for_array<U>()>>
    mut_span_t& operator/=(const T2& rhs) noexcept(is_scalar_v<T2>) {
        auto* x = data();
        const int n = size();
        using R = ResultType<T, T2>;
        static_assert(std::is_same_v<T, R>, "The operation changes the type");
        if constexpr (is_scalar_v<T2>) {
            for (int i = 0; i < n; ++i) {
                x[i] /= rhs;
            }
        } else {
            DSPLIB_ASSERT(n == rhs.size(), "Array lengths must be equal");
            for (int i = 0; i < n; ++i) {
                x[i] /= rhs[i];
            }
        }
        return *this;
    }

    //---------------------------------------------------------------------------
    template<class T2>
    auto operator+(const T2& rhs) const {
        return span_t<T>(*this) + rhs;
    }

    template<class T2>
    auto operator-(const T2& rhs) const {
        return span_t<T>(*this) - rhs;
    }

    template<class T2>
    auto operator*(const T2& rhs) const {
        return span_t<T>(*this) * rhs;
    }

    template<class T2>
    auto operator/(const T2& rhs) const {
        return span_t<T>(*this) / rhs;
    }

private:
    bool is_same_memory(span_t<T> rhs) noexcept {
        if (this->size() == 0 || rhs.size() == 0) {
            return false;
        }
        auto start1 = this->data();
        auto end1 = start1 + this->size();
        auto start2 = rhs.data();
        auto end2 = start2 + rhs.size();
        return (start1 < end2) && (start2 < end1);
    }
};

//immutable span
template<typename T>
class span_t : public slice_t<T>
{
public:
    static_assert(!std::is_same_v<std::remove_cv_t<T>, bool>, "`bool` type is not supported");
    static_assert(std::is_trivially_copyable<T>(), "type must be trivially copyable");

    friend class mut_span_t<T>;

    span_t() = default;

    explicit span_t(const T* data, int size)
      : slice_t<T>{data, 1, size} {
    }

    span_t(const mut_span_t<T>& v)
      : span_t(v.data(), v.size()) {
    }

    template<typename U = T, typename = std::enable_if_t<support_type_for_array<U>()>>
    span_t(const base_array<T>& v)
      : span_t(v.data(), v.size()) {
    }

    span_t(const std::vector<T>& v)
      : span_t(v.data(), v.size()) {
    }

    [[nodiscard]] const T* data() const noexcept {
        return this->data_;
    }

    [[nodiscard]] int size() const noexcept {
        return this->count_;
    }

    const T& operator[](int i) const noexcept {
        assert((i >= 0) && (i < size()));
        return this->data_[i];
    }

    using const_iterator = const T*;

    const_iterator begin() const noexcept {
        return this->data_;
    }

    const_iterator end() const noexcept {
        return this->data_ + this->count_;
    }

    span_t slice(int i1, int i2) const {
        DSPLIB_ASSERT((i1 >= 0) && (i1 < this->count_), "invalid range");
        DSPLIB_ASSERT((i2 > i1) && (i2 <= this->count_), "invalid range");
        return span_t(this->data_ + i1, (i2 - i1));
    }

    //---------------------------------------------------------------------------
    //arithmetic operators
    template<class T2>
    auto operator+(const T2& rhs) const {
        auto* x = data();
        const int n = size();
        using R = ResultType<T, T2>;
        base_array<R> res(n);
        if constexpr (is_scalar_v<T2>) {
            for (int i = 0; i < n; ++i) {
                res[i] = x[i] + rhs;
            }
        } else {
            DSPLIB_ASSERT(n == rhs.size(), "Array lengths must be equal");
            for (int i = 0; i < n; ++i) {
                res[i] = x[i] + rhs[i];
            }
        }
        return res;
    }

    template<class T2>
    auto operator-(const T2& rhs) const {
        auto* x = data();
        const int n = size();
        using R = ResultType<T, T2>;
        base_array<R> res(n);
        if constexpr (is_scalar_v<T2>) {
            for (int i = 0; i < n; ++i) {
                res[i] = x[i] - rhs;
            }
        } else {
            DSPLIB_ASSERT(n == rhs.size(), "Array lengths must be equal");
            for (int i = 0; i < n; ++i) {
                res[i] = x[i] - rhs[i];
            }
        }
        return res;
    }

    template<class T2>
    auto operator*(const T2& rhs) const {
        auto* x = data();
        const int n = size();
        using R = ResultType<T, T2>;
        base_array<R> res(n);
        if constexpr (is_scalar_v<T2>) {
            for (int i = 0; i < n; ++i) {
                res[i] = x[i] * rhs;
            }
        } else {
            DSPLIB_ASSERT(n == rhs.size(), "Array lengths must be equal");
            for (int i = 0; i < n; ++i) {
                res[i] = x[i] * rhs[i];
            }
        }
        return res;
    }

    template<class T2>
    auto operator/(const T2& rhs) const {
        auto* x = data();
        const int n = size();
        using R = ResultType<T, T2>;
        base_array<R> res(n);
        if constexpr (is_scalar_v<T2>) {
            for (int i = 0; i < n; ++i) {
                res[i] = x[i] / rhs;
            }
        } else {
            DSPLIB_ASSERT(n == rhs.size(), "Array lengths must be equal");
            for (int i = 0; i < n; ++i) {
                res[i] = x[i] / rhs[i];
            }
        }
        return res;
    }
};

//------------------------------------------------------------------------------------------------
template<typename T>
span_t<T> make_span(const T* x, int nx) noexcept {
    return span_t<T>(x, nx);
}

template<typename T>
mut_span_t<T> make_span(T* x, int nx) noexcept {
    return mut_span_t<T>(x, nx);
}

template<typename T>
span_t<T> make_span(const std::vector<T>& x) noexcept {
    return span_t<T>(x.data(), x.size());
}

template<typename T>
mut_span_t<T> make_span(std::vector<T>& x) noexcept {
    return mut_span_t<T>(x.data(), x.size());
}

template<typename T>
span_t<T> make_span(const base_array<T>& x) noexcept {
    return span_t<T>(x.data(), x.size());
}

template<typename T>
mut_span_t<T> make_span(base_array<T>& x) noexcept {
    return mut_span_t<T>(x.data(), x.size());
}

using span_real = span_t<real_t>;
using span_cmplx = span_t<cmplx_t>;

template<typename T>
class inplace_span_t
{
public:
    explicit inplace_span_t(mut_span_t<T> v) noexcept
      : d_{std::move(v)} {
    }

    mut_span_t<T> get() noexcept {
        return d_;
    }

private:
    mut_span_t<T> d_;
};

template<typename T>
inplace_span_t<T> inplace(base_array<T>& x) {
    return inplace_span_t(make_span(x));
}

template<typename T>
inplace_span_t<T> inplace(std::vector<T>& x) {
    return inplace_span_t(make_span(x));
}

template<typename T>
inplace_span_t<T> inplace(mut_span_t<T> x) {
    return inplace_span_t(x);
}

using inplace_real = inplace_span_t<real_t>;
using inplace_cmplx = inplace_span_t<cmplx_t>;

}   // namespace dsplib