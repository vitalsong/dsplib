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

    slice_t(const slice_t& rhs)
      : data_{rhs.data_}
      , stride_{rhs.stride_}
      , count_{rhs.count_} {
    }

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

    [[deprecated("use `copy` instead")]] base_array<T> operator*() const noexcept {
        return base_array<T>(*this);
    }

    static slice_t make_slice(const T* data, int size, int i1, int i2, int step) {
        auto mdata = const_cast<T*>(data);
        return slice_t(mut_slice_t<T>::make_slice(mdata, size, i1, i2, step));
    }

    base_array<T> copy() const noexcept {
        return base_array<T>(*this);
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

    mut_slice_t(const mut_slice_t& rhs)
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

    mut_slice_t& operator=(const slice_t<T>& rhs) {
        this->assign(rhs);
        return *this;
    }

    mut_slice_t& operator=(const mut_slice_t<T>& rhs) {
        this->assign(slice_t<T>(rhs));
        return *this;
    }

    mut_slice_t& operator=(const base_array<T>& rhs) {
        DSPLIB_ASSERT(!is_same_memory(rhs.slice(0, rhs.size())), "Assigned array to same slice");
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

    [[deprecated("use `copy` instead")]] base_array<T> operator*() const noexcept {
        return base_array<T>(*this);
    }

    base_array<T> copy() const noexcept {
        return base_array<T>(*this);
    }

    void assign(slice_t<T> rhs) {
        DSPLIB_ASSERT(size() == rhs.size(), "Slices size must be equal");
        const int count = size();

        //empty slice assign
        if (count == 0) {
            return;
        }

        //simple block copy/move (optimization)
        const bool is_same = is_same_memory(rhs);

        //check all slices is span
        if ((stride() == 1) && (rhs.stride() == 1)) {
            const auto* src = rhs.data_;
            auto* dst = data_;
            if (!is_same) {
                std::memcpy(dst, src, count * sizeof(*src));
            } else {
                std::memmove(dst, src, count * sizeof(*src));
            }
            return;
        }

        //same array, specific indexing
        if (is_same) {
            *this = base_array<T>(rhs);
            return;
        }

        std::copy(rhs.begin(), rhs.end(), begin());
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

protected:
    explicit mut_slice_t(T* data, int stride, int count)
      : data_{data}
      , stride_{stride}
      , count_{count} {
        DSPLIB_ASSERT(count >= 0, "Count of elements must be positive");
    }

    bool is_same_memory(slice_t<T> rhs) noexcept {
        if (empty() || rhs.empty()) {
            return false;
        }
        auto start1 = rhs.data_;
        auto end1 = start1 + (rhs.count_ * rhs.stride_);
        if (start1 > end1) {
            std::swap(start1, end1);
        }

        auto start2 = data_;
        auto end2 = start2 + (count_ * stride_);
        if (start2 > end2) {
            std::swap(start2, end2);
        }

        return (start1 < end2) && (start2 < end1);
    }

    T* data_{nullptr};
    int stride_{0};
    int count_{0};
};

}   // namespace dsplib