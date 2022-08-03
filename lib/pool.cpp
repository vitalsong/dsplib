#include <dsplib/pool.h>
#include <dsplib/math.h>
#include <vector>

namespace dsplib {

constexpr int MAX_POOLED_SIZE = (1L << 15);
constexpr int BLOCK_SIZE = 512;
constexpr int STORAGE_SIZE = MAX_POOLED_SIZE / BLOCK_SIZE + 1;

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
//(size, count)
using vec_pool_t = storage_t<std::vector<real_t>>;
static thread_local std::vector<vec_pool_t> _storage(STORAGE_SIZE);
static thread_local size_t _bytes_allocated{0};

static int key_from_size(int size) {
    return (size % BLOCK_SIZE == 0) ? (size / BLOCK_SIZE) : (size / BLOCK_SIZE + 1);
}

//---------------------------------------------------------------------------------------
template<>
block_t<real_t> vec_pool<real_t>::get(int size) {
    if ((size > MAX_POOLED_SIZE) or (_bytes_allocated > MEMORY_ALLOCATION_LIMIT)) {
        return block_t<real_t>(size);
    }

    const int key = key_from_size(size);
    const int n = key * BLOCK_SIZE;
    if (_storage[key].size() == 0) {
        _storage[key].push(std::vector<real_t>(n));
        _bytes_allocated += n * sizeof(real_t);
    }

    return block_t<real_t>(size, _storage[key].pop(), true);
}

//---------------------------------------------------------------------------------------
template<>
block_t<cmplx_t> vec_pool<cmplx_t>::get(int size) {
    auto block = vec_pool<real_t>::get(size * 2);
    block_t<cmplx_t> res(size, std::move(block._raw), block._use_pool);
    block._use_pool = false;
    return res;
}

//----------------------------------------------------------------------------------------
template<>
block_t<real_t>::~block_t() {
    if (_use_pool) {
        const int key = key_from_size(_raw.size());
        //TODO: perfomance problem
        std::fill(_raw.begin(), _raw.end(), 0);
        _storage[key].push(std::move(_raw));
    }
}

template<>
block_t<cmplx_t>::~block_t() {
    if (_use_pool) {
        const int key = key_from_size(_raw.size());
        std::fill(_raw.begin(), _raw.end(), 0);
        _storage[key].push(std::move(_raw));
    }
}

}   // namespace dsplib