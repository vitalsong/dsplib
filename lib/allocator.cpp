#include <dsplib/allocator.h>

#include <memory>
#include <vector>
#include <cassert>
#include <unordered_map>
#include <stdexcept>

namespace dsplib {

//----------------------------------------------------------------------------------------
constexpr int MAX_POOLED_SIZE = (1L << 15);
constexpr int SMALL_BLOCK_SIZE = 64;
constexpr int BLOCK_SIZE = 512;
constexpr int STORAGE_COUNT = MAX_POOLED_SIZE / BLOCK_SIZE + 1;
constexpr size_t MEMORY_ALLOCATION_LIMIT = 1024 * 1024;   //1 MB (TODO: set from options)
constexpr int NOT_POOLED_KEY = -1;

//----------------------------------------------------------------------------------------
class storage_t
{
public:
    void push(void* e) {
        _pool.push_back(e);
    }

    void* pop() {
        auto e = _pool.back();
        _pool.pop_back();
        return e;
    }

    size_t size() const {
        return _pool.size();
    }

private:
    std::vector<void*> _pool;
};

//----------------------------------------------------------------------------------------
static thread_local std::vector<storage_t> _storage(STORAGE_COUNT);
static thread_local size_t _bytes_allocated{0};
static thread_local std::unordered_map<void*, int> _alocate_map;

static int key_by_size(int size) {
    assert(size <= MAX_POOLED_SIZE);
    if (size <= SMALL_BLOCK_SIZE) {
        return 0;
    }
    return (size % BLOCK_SIZE == 0) ? (size / BLOCK_SIZE) : (size / BLOCK_SIZE + 1);
}

static int size_by_key(int key) {
    assert((key >= 0) && (key < STORAGE_COUNT));
    if (key == 0) {
        return SMALL_BLOCK_SIZE;
    }
    return key * BLOCK_SIZE;
}

//----------------------------------------------------------------------------------------
void pool_init(std::vector<int> map) {
    int bytes_needed = 0;
    for (auto size : map) {
        int pool_size = size_by_key(key_by_size(size));
        bytes_needed += pool_size;
    }

    if ((bytes_needed + _bytes_allocated) > MEMORY_ALLOCATION_LIMIT) {
        throw std::runtime_error("Limit is exceeded. Memory has not been allocated.");
    }

    //warm up the memory pool
    for (auto size : map) {
        auto ptr = pool_alloc(size);
        pool_free(ptr);
    }
}

//----------------------------------------------------------------------------------------
void pool_reset() {
    for (int key = 0; key < _storage.size(); ++key) {
        auto& pool = _storage[key];
        while (pool.size() > 0) {
            auto ptr = pool.pop();
            free(ptr);
            _bytes_allocated -= size_by_key(key);
        }
    }
}

//----------------------------------------------------------------------------------------
void* pool_alloc(size_t size) {
    if (size > MAX_POOLED_SIZE) {
        auto ptr = malloc(size);
        _alocate_map[ptr] = NOT_POOLED_KEY;
        return ptr;
    }

    const int key = key_by_size(size);
    auto& pool = _storage[key];
    const int cap = size_by_key(key);

    //TODO: get first free object from pool (not equal size)
    if ((pool.size() == 0) && (_bytes_allocated > MEMORY_ALLOCATION_LIMIT)) {
        auto ptr = malloc(size);
        _alocate_map[ptr] = NOT_POOLED_KEY;
        return ptr;
    }

    //add block to pool
    if (pool.size() == 0) {
        auto ptr = malloc(cap);
        pool.push(ptr);
        _bytes_allocated += cap;
    }

    auto ptr = pool.pop();
    _alocate_map[ptr] = key;
    return ptr;
}

//----------------------------------------------------------------------------------------
void pool_free(void* ptr) {
    assert(_alocate_map.count(ptr) == 1);
    auto key = _alocate_map.at(ptr);
    _alocate_map.erase(ptr);
    if (key == NOT_POOLED_KEY) {
        free(ptr);
    } else {
        auto& pool = _storage[key];
        pool.push(ptr);
    }
}

std::vector<pool_info_t> pool_info() {
    std::vector<pool_info_t> out;

    //free blocks
    for (int k = 0; k < _storage.size(); ++k) {
        auto& pool = _storage[k];
        for (int i = 0; i < pool.size(); ++i) {
            auto pool_size = size_by_key(k);
            pool_info_t info = {false, pool_size};
            out.push_back(info);
        }
    }

    //used blocks
    for (auto it : _alocate_map) {
        auto key = it.second;
        auto pool_size = size_by_key(key);
        pool_info_t info = {true, pool_size};
        out.push_back(info);
    }

    return out;
}

}   // namespace dsplib