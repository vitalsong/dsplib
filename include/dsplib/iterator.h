#include <iterator>
#include <type_traits>

namespace dsplib {

template<typename T>
struct SliceIterator
{
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;
    using value_type = std::remove_cv_t<T>;

    SliceIterator(pointer ptr, int step = 1)
      : ptr_{ptr}
      , step_{step} {
    }

    reference operator*() const noexcept {
        return *ptr_;
    }

    pointer operator->() noexcept {
        return ptr_;
    }

    SliceIterator& operator++() noexcept {
        ptr_ += step_;
        return *this;
    }

    SliceIterator operator++(int) noexcept {
        SliceIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    SliceIterator& operator--() noexcept {
        ptr_ -= step_;
        return *this;
    }

    SliceIterator operator--(int) noexcept {
        SliceIterator tmp = *this;
        --(*this);
        return tmp;
    }

    friend bool operator==(const SliceIterator& lhs, const SliceIterator& rhs) noexcept {
        return lhs.ptr_ == rhs.ptr_;
    };

    friend bool operator!=(const SliceIterator& lhs, const SliceIterator& rhs) noexcept {
        return lhs.ptr_ != rhs.ptr_;
    };

private:
    pointer ptr_;
    int step_{1};
};

}   // namespace dsplib