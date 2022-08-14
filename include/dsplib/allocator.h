#pragma once

#include <vector>
#include <memory>
#include <cassert>

namespace dsplib {

// Implementation of an allocator as a pool of memory blocks.
// Used to allocate memory for a vector on platforms where frequent malloc() calls are critical.
// For each memory request, a free block is checked.
// If there are no free blocks, then memory is allocated using malloc(), the pointer is placed in the pool
// and returned as a temporary object.
// When the memory allocation limit is exceeded, the allocation will be performed in the standard way.
// Block sizes are multiples of 512 bytes, i.e. 512, 1024, 1536 ... MAX_POOLED_SIZE
// There is also a separate pool of 64 bytes for small objects.
// Alignment of the real_t/cmplx_t pointer is guaranteed by the malloc implementation.

template<class T>
inline void unused(T&&) {
}

// Init pool table
// Prepare a pool of objects if the block sizes are known in advance.
// For evaluation, you can use function pool_info() after running your algorithm.
void pool_init(std::vector<int> map);

// Release all free memory blocks
void pool_reset();

// Allocate memory block from pool
void* pool_alloc(size_t size);

// Free (return) memory block to pool
void pool_free(void* ptr);

struct pool_info_t
{
    pool_info_t(bool used, int size)
      : used(used)
      , size(size) {
    }
    bool used{false};
    int size{0};
};

// Current pool state
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
        auto ptr = pool_alloc(n * sizeof(T));
        auto r_ptr = reinterpret_cast<pointer>(ptr);
        assert(uintptr_t(ptr) == uintptr_t(r_ptr));
        return r_ptr;
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