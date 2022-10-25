#pragma once

#include <mutex>
#include <map>

#include <dsplib/throw.h>

namespace dsplib {

template<typename K, typename T>
class datacache
{
public:
    datacache() = default;

    void update(K key, const T& value) {
        std::lock_guard<std::mutex> lk(_mtx);
        _cache[key] = value;
    }

    T get(K key) const {
        std::lock_guard<std::mutex> lk(_mtx);
        if (_cache.count(key) == 0) {
            DSPLIB_THROW("error getting value. the data has not been cached.");
        }
        return _cache.at(key);
    }

    bool cached(K key) const {
        std::lock_guard<std::mutex> lk(_mtx);
        return _cache.count(key) != 0;
    }

    void reset(K key) {
        std::lock_guard<std::mutex> lk(_mtx);
        if (_cache.count(key) == 0) {
            return;
        }
        _cache.erase(key);
    }

private:
    std::map<K, T> _cache;
    mutable std::mutex _mtx;
};

}   // namespace dsplib