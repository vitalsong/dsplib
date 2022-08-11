#pragma once

#include <vector>
#include <memory>
#include <stdio.h>

namespace dsplib {

//allocate memory block from pool
void* pool_alloc(size_t size);

//free (return) memory block to pool
void pool_free(void* ptr);

//current pool state
//TODO: allocated/free
std::vector<int> pool_info();

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
        return reinterpret_cast<pointer>(pool_alloc(n * sizeof(T)));
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