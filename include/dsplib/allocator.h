#pragma once

#include <memory>
#include <stdio.h>

#include <dsplib/mempool.h>

namespace dsplib {

template<typename T>
class allocator : public std::allocator<T>
{
public:
    typedef size_t size_type;
    typedef T* pointer;
    typedef const T* const_pointer;

    template<typename _Tp1>
    struct rebind
    { typedef allocator<_Tp1> other; };

    pointer allocate(size_type n, const void* hint = 0) {
        return (pointer)pool_alloc(n * sizeof(T));
    }

    void deallocate(pointer p, size_type n) {
        pool_free(p);
    }

    allocator() noexcept
      : std::allocator<T>() {
    }

    allocator(const allocator& a) noexcept
      : std::allocator<T>(a) {
    }

    template<class U>
    allocator(const allocator<U>& a) noexcept
      : std::allocator<T>(a) {
    }

    ~allocator() noexcept {
    }
};

}   // namespace dsplib