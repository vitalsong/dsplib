#pragma once

#include <dsplib/types.h>

#include <vector>

namespace dsplib {

template<class T>
class vec_pool;

//-------------------------------------------------------------
template<typename T>
class block_t
{
public:
    friend vec_pool<real_t>;
    friend vec_pool<cmplx_t>;

    ~block_t();

    block_t(std::vector<real_t>&& raw)
      : _size{static_cast<int>(raw.size())}
      , _use_pool{false}
      , _raw{std::move(raw)}
      , _data{reinterpret_cast<T*>(_raw.data())} {
    }

    block_t(int size)
      : block_t(std::move(std::vector<real_t>(size))) {
    }

    block_t(block_t&& rhs)
      : _size{rhs._size}
      , _use_pool{rhs._use_pool}
      , _raw{std::move(rhs._raw)}
      , _data{reinterpret_cast<T*>(_raw.data())} {
        rhs._use_pool = false;
        rhs._size = 0;
        rhs._data = nullptr;
    }

    block_t(const block_t& rhs) = delete;
    block_t& operator=(const block_t& rhs) = delete;

    //--------------------------------------------------------------------
    const T& operator[](int i) const {
        return _data[i];
    }

    T& operator[](int i) {
        return _data[i];
    }

    T* data() const {
        return _data;
    }

    T* data() {
        return _data;
    }

    int size() const {
        return _size;
    }

    bool empty() const {
        return _size == 0;
    }

    typedef typename std::vector<T>::iterator iterator;
    typedef typename std::vector<T>::const_iterator const_iterator;

    iterator begin() {
        return _data;
    }

    iterator end() {
        return (_data + _size);
    }

    const_iterator begin() const {
        return _data;
    }

    const_iterator end() const {
        return (_data + _size);
    }

private:
    block_t(int size, std::vector<real_t>&& raw, bool use_pool = false)
      : _size{size}
      , _use_pool{use_pool}
      , _raw{std::move(raw)}
      , _data{reinterpret_cast<T*>(_raw.data())} {
    }

    int _size{0};
    bool _use_pool{false};
    std::vector<real_t> _raw;
    T* _data{nullptr};
};

template<>
block_t<real_t>::~block_t();

template<>
block_t<cmplx_t>::~block_t();

//-------------------------------------------------------------
template<class T>
class vec_pool
{
public:
    static block_t<T> get(int size);
    //TODO: max block size
    //TODO: max allocate size
};

template<>
block_t<real_t> vec_pool<real_t>::get(int size);

template<>
block_t<cmplx_t> vec_pool<cmplx_t>::get(int size);

}   // namespace dsplib