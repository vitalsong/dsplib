#pragma once

#include <dsplib/types.h>

#include <vector>

namespace dsplib {

//-------------------------------------------------------------
template<typename T>
class vector_pool_t
{
public:
    friend vector_pool_t<real_t>;
    friend vector_pool_t<cmplx_t>;

    vector_pool_t(int size)
      : vector_pool_t{instance(size)} {
    }

    ~vector_pool_t();

    vector_pool_t(std::vector<real_t>&& raw)
      : _size{static_cast<int>(raw.size())}
      , _use_pool{false}
      , _raw{std::move(raw)}
      , _data{reinterpret_cast<T*>(_raw.data())} {
    }

    vector_pool_t(vector_pool_t&& rhs)
      : _size{rhs._size}
      , _use_pool{rhs._use_pool}
      , _raw{std::move(rhs._raw)}
      , _data{reinterpret_cast<T*>(_raw.data())} {
        rhs._use_pool = false;
        rhs._size = 0;
        rhs._data = nullptr;
    }

    vector_pool_t(const vector_pool_t& rhs) = delete;
    vector_pool_t& operator=(const vector_pool_t& rhs) = delete;

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
    vector_pool_t(int size, std::vector<real_t>&& raw, bool use_pool = false)
      : _size{size}
      , _use_pool{use_pool}
      , _raw{std::move(raw)}
      , _data{reinterpret_cast<T*>(_raw.data())} {
    }

    static vector_pool_t<T> instance(int size);

    int _size{0};
    bool _use_pool{false};
    std::vector<real_t> _raw;
    T* _data{nullptr};
};

template<>
vector_pool_t<real_t>::~vector_pool_t();

template<>
vector_pool_t<cmplx_t>::~vector_pool_t();

template<>
vector_pool_t<real_t> vector_pool_t<real_t>::instance(int size);

template<>
vector_pool_t<cmplx_t> vector_pool_t<cmplx_t>::instance(int size);

}   // namespace dsplib