#pragma once

#include <vector>
#include <memory>

namespace dsplib {

template<class T>
inline void unused(T&&) {
}

//init pool table
void pool_init(std::vector<int> map);

//release all free memory blocks
void pool_reset();

//allocate memory block from pool
void* pool_alloc(size_t size);

//free (return) memory block to pool
void pool_free(void* ptr);

//current pool state
//TODO: allocated/free
struct pool_info_t
{
    pool_info_t(bool used, int size)
      : used(used)
      , size(size) {
    }
    bool used{false};
    int size{0};
};

std::vector<pool_info_t> pool_info();

template<typename T>
class pool_allocator : public std::allocator<T>
{
public:
    typedef size_t size_type;
    typedef T* pointer;
    typedef const T* const_pointer;

    template<typename _Tp1>
    struct rebind
    { typedef pool_allocator<_Tp1> other; };

    pointer allocate(size_type n, const void* hint = 0) {
        unused(hint);
        return reinterpret_cast<pointer>(pool_alloc(n * sizeof(T)));
    }

    void deallocate(pointer p, size_type n) {
        unused(n);
        pool_free(p);
    }

    pool_allocator() noexcept
      : std::allocator<T>() {
    }

    pool_allocator(const pool_allocator& a) noexcept
      : std::allocator<T>(a) {
    }

    template<class U>
    pool_allocator(const pool_allocator<U>& a) noexcept
      : std::allocator<T>(a) {
    }

    ~pool_allocator() noexcept {
    }
};

}   // namespace dsplib