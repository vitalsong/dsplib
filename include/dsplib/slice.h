#pragma once

#include <algorithm>
#include <cstring>

#include <dsplib/assert.h>
#include <dsplib/iterator.h>

namespace dsplib {

template<typename T>
class base_array;

template<typename T>
class mut_slice_t;

template<typename T>
class slice_t;

/**
 * @brief Non-mutable slice object
 * @todo add concatenate array = slice | array
 * @tparam T real_t/cmplx_t
 */
template<typename T>
class slice_t
{
public:
    friend class mut_slice_t<T>;
    using const_iterator = SliceIterator<const T>;

    slice_t() = default;

    slice_t(const mut_slice_t<T>& rhs)
      : data_{rhs.data_}
      , stride_{rhs.stride_}
      , count_{rhs.count_} {
    }

    [[nodiscard]] int size() const noexcept {
        return count_;
    }

    bool empty() const noexcept {
        return count_ == 0;
    }

    [[nodiscard]] int stride() const noexcept {
        return stride_;
    }

    const_iterator begin() const noexcept {
        return const_iterator(data_, stride_);
    }

    const_iterator end() const noexcept {
        auto current = begin();
        std::advance(current, count_);
        return current;
    }

    base_array<T> operator*() const noexcept {
        return base_array<T>(*this);
    }

    static slice_t make_slice(const T* data, int size, int i1, int i2, int step) {
        auto mdata = const_cast<T*>(data);
        return slice_t(mut_slice_t<T>::make_slice(mdata, size, i1, i2, step));
    }

protected:
    explicit slice_t(const T* data, int stride, int count)
      : data_{data}
      , stride_{stride}
      , count_{count} {
        DSPLIB_ASSERT(count >= 0, "Count of elements must be positive");
    }

    const T* data_{nullptr};
    int stride_{0};
    int count_{0};
};

/**
 * @brief Mutable slice object
 * @tparam T real_t/cmplx_t
 */
template<typename T>
class mut_slice_t
{
public:
    friend class slice_t<T>;
    using iterator = SliceIterator<T>;
    using const_iterator = SliceIterator<const T>;

    mut_slice_t() = default;

    [[nodiscard]] int size() const noexcept {
        return count_;
    }

    bool empty() const noexcept {
        return count_ == 0;
    }

    [[nodiscard]] int stride() const noexcept {
        return stride_;
    }

    mut_slice_t& operator=(const slice_t<T>& rhs) {
        this->copy(*this, rhs);
        return *this;
    }

    mut_slice_t& operator=(const mut_slice_t<T>& rhs) {
        *this = slice_t<T>(rhs);
        return *this;
    }

    mut_slice_t& operator=(const base_array<T>& rhs) {
        DSPLIB_ASSERT(!is_same_memory(rhs.slice(0, rhs.size()), *this), "Assigned array to same slice");
        *this = rhs.slice(0, rhs.size());
        return *this;
    }

    mut_slice_t& operator=(const T& value) {
        std::fill(begin(), end(), value);
        return *this;
    }

    mut_slice_t& operator=(const std::initializer_list<T>& rhs) {
        std::copy(rhs.begin(), rhs.end(), this->begin());
        return *this;
    }

    iterator begin() noexcept {
        return iterator(data_, stride_);
    }

    iterator end() noexcept {
        auto current = begin();
        std::advance(current, count_);
        return current;
    }

    const_iterator begin() const noexcept {
        return const_iterator(data_, stride_);
    }

    const_iterator end() const noexcept {
        auto current = begin();
        std::advance(current, count_);
        return current;
    }

    base_array<T> operator*() const noexcept {
        return base_array<T>(*this);
    }

    static bool is_same_memory(slice_t<T> r1, slice_t<T> r2) noexcept {
        if (r1.empty() || r2.empty()) {
            return false;
        }
        auto start1 = r1.begin();
        auto end1 = r1.end();
        auto start2 = r2.begin();
        auto end2 = r2.end();
        return (start1 < end2) && (start2 < end1);
    }

    static mut_slice_t make_slice(T* data, int size, int i1, int i2, int step) {
        if (size == 0) {
            return mut_slice_t();
        }

        i1 = (i1 < 0) ? (size + i1) : i1;
        i2 = (i2 < 0) ? (size + i2) : i2;

        const int d = std::abs(i2 - i1);
        const int tm = std::abs(step);
        int count = (d % tm != 0) ? (d / tm + 1) : (d / tm);

        DSPLIB_ASSERT(step != 0, "Slice stride cannot be zero");
        DSPLIB_ASSERT((i1 >= 0) && (i1 < size), "Left slice index out of range");
        DSPLIB_ASSERT((i2 >= 0) && (i2 <= size), "Right slice index out of range");
        DSPLIB_ASSERT(!((step < 0) && (i1 < i2)), "First index is smaller for negative step");
        DSPLIB_ASSERT(!((step > 0) && (i1 > i2)), "First index is greater for positive step");
        DSPLIB_ASSERT(count <= size, "Slice range is greater array size");

        return mut_slice_t(data + i1, step, count);
    }

    static void copy(mut_slice_t<T> lhs, slice_t<T> rhs) {
        DSPLIB_ASSERT(lhs.size() == rhs.size(), "Slices size must be equal");
        const int count = lhs.size();

        //empty slice assign
        if (count == 0) {
            return;
        }

        //simple block copy/move (optimization)
        const bool is_same = is_same_memory(rhs, slice_t(lhs));
        if ((lhs.stride() == 1) && (rhs.stride() == 1)) {
            const auto* src = &(*rhs.begin());
            auto* dst = &(*lhs.begin());
            if (!is_same) {
                std::memcpy(dst, src, count * sizeof(*src));
            } else {
                //overlapped
                std::memmove(dst, src, count * sizeof(*src));
            }
            return;
        }

        //same array, specific indexing
        if (is_same) {
            lhs = base_array<T>(rhs);
            return;
        }

        std::copy(rhs.begin(), rhs.end(), lhs.begin());
    }

protected:
    explicit mut_slice_t(T* data, int stride, int count)
      : data_{data}
      , stride_{stride}
      , count_{count} {
        DSPLIB_ASSERT(count >= 0, "Count of elements must be positive");
    }

    T* data_{nullptr};
    int stride_{0};
    int count_{0};
};

}   // namespace dsplib