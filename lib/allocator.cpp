#include <dsplib/allocator.h>

#include <memory>
#include <vector>
#include <cassert>
#include <unordered_map>

namespace dsplib {

//----------------------------------------------------------------------------------------
constexpr int MAX_POOLED_SIZE = (1L << 15);
constexpr int BLOCK_SIZE = 512;
constexpr int STORAGE_COUNT = MAX_POOLED_SIZE / BLOCK_SIZE + 1;
constexpr size_t MEMORY_ALLOCATION_LIMIT = 1024 * 1024;   //1 MB

//----------------------------------------------------------------------------------------
template<class T>
class storage_t
{
public:
    void push(T&& e) {
        _pool.push_back(std::move(e));
    }

    T pop() {
        auto e = std::move(_pool.back());
        _pool.pop_back();
        return e;
    }

    size_t size() const {
        return _pool.size();
    }

private:
    std::vector<T> _pool;
};

//----------------------------------------------------------------------------------------
using vec_pool_t = storage_t<void*>;
static thread_local std::vector<vec_pool_t> _storage(STORAGE_COUNT);
static thread_local size_t _bytes_allocated{0};
static thread_local std::unordered_map<void*, int> _alocate_map;

constexpr int key_from_size(int size) {
    return (size % BLOCK_SIZE == 0) ? (size / BLOCK_SIZE) : (size / BLOCK_SIZE + 1);
}

//----------------------------------------------------------------------------------------
void* pool_alloc(size_t size) {
    if (size > MAX_POOLED_SIZE) {
        auto ptr = malloc(size);
        _alocate_map[ptr] = 0;
        return ptr;
    }

    const int key = key_from_size(size);
    auto& pool = _storage[key];
    const int cap = key * BLOCK_SIZE;

    //TODO: get first free object from pool (not equal size)
    if ((pool.size() == 0) and (_bytes_allocated > MEMORY_ALLOCATION_LIMIT)) {
        auto ptr = malloc(size);
        _alocate_map[ptr] = 0;
        return ptr;
    }

    if (pool.size() == 0) {
        auto ptr = malloc(cap);
        _alocate_map[ptr] = key;
        pool.push(std::move(ptr));
        _bytes_allocated += cap;
    }

    return pool.pop();
}

//----------------------------------------------------------------------------------------
void pool_free(void* ptr) {
    assert(_alocate_map.count(ptr) != 0);
    auto key = _alocate_map[ptr];
    if (key == 0) {
        free(ptr);
    } else {
        auto& pool = _storage[key];
        pool.push(std::move(ptr));
    }
}

std::vector<int> pool_info() {
    std::vector<int> out;
    for (int k = 0; k < _storage.size(); ++k) {
        for (int i = 0; i < _storage[k].size(); ++i) {
            out.push_back(k * BLOCK_SIZE);
        }
    }
    return out;
}

}   // namespace dsplib