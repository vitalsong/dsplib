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
vector_pool_t<real_t> vector_pool_t<real_t>::instance(int size) {
    if (size == 0) {
        return vector_pool_t<real_t>(std::vector<real_t>(size));
    }

    const int key = key_from_size(size);
    auto& pool = _storage[key];
    if ((size > MAX_POOLED_SIZE) or (pool.size() == 0 and _bytes_allocated > MEMORY_ALLOCATION_LIMIT)) {
        return vector_pool_t<real_t>(std::vector<real_t>(size));
    }

    //TODO: get first free object from pool (not equal size)
    //...

    const int n = key * BLOCK_SIZE;
    if (pool.size() == 0) {
        pool.push(std::vector<real_t>(n));
        _bytes_allocated += n * sizeof(real_t);
    }

    return vector_pool_t<real_t>(size, pool.pop(), true);
}

//---------------------------------------------------------------------------------------
template<>
vector_pool_t<cmplx_t> vector_pool_t<cmplx_t>::instance(int size) {
    auto block = vector_pool_t<real_t>::instance(size * 2);
    vector_pool_t<cmplx_t> res(size, std::move(block._raw), block._use_pool);
    block._use_pool = false;
    return res;
}

//----------------------------------------------------------------------------------------
template<>
vector_pool_t<real_t>::~vector_pool_t() {
    if (_use_pool) {
        const int key = key_from_size(_raw.size());
        //TODO: perfomance problem
        std::fill(_raw.begin(), _raw.end(), 0);
        _storage[key].push(std::move(_raw));
    }
}

template<>
vector_pool_t<cmplx_t>::~vector_pool_t() {
    if (_use_pool) {
        const int key = key_from_size(_raw.size());
        std::fill(_raw.begin(), _raw.end(), 0);
        _storage[key].push(std::move(_raw));
    }
}

std::vector<int> vector_pool_state() {
    std::vector<int> out;
    for (int k = 0; k < _storage.size(); ++k) {
        for (int i = 0; i < _storage[k].size(); ++i) {
            out.push_back(k * BLOCK_SIZE);
        }
    }
    return out;
}

}   // namespace dsplib