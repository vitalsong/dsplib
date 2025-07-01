#pragma once

#include <list>
#include <unordered_map>
#include <cassert>
#include <cstddef>

namespace dsplib {

template<typename Key, typename Value>
class LRUCache
{
public:
    using KeyValue_t = std::pair<Key, Value>;
    using ListIterator_t = typename std::list<KeyValue_t>::iterator;

    explicit LRUCache(size_t max_size)
      : max_size_(max_size) {
    }

    void put(const Key& key, const Value& value) {
        auto it = items_map_.find(key);
        items_list_.push_front(KeyValue_t(key, value));
        if (it != items_map_.end()) {
            items_list_.erase(it->second);
            items_map_.erase(it);
        }

        items_map_[key] = items_list_.begin();
        if (items_map_.size() > max_size_) {
            auto last = items_list_.end();
            last--;
            items_map_.erase(last->first);
            items_list_.pop_back();
        }
    }

    const Value& get(const Key& key) {
        auto it = items_map_.find(key);
        if (it == items_map_.end()) {
            DSPLIB_THROW("There is no such key in cache");
        }
        items_list_.splice(items_list_.begin(), items_list_, it->second);
        return it->second->second;
    }

    bool exists(const Key& key) const {
        return items_map_.find(key) != items_map_.end();
    }

    [[nodiscard]] int size() const {
        return items_map_.size();
    }

private:
    std::list<KeyValue_t> items_list_;
    std::unordered_map<Key, ListIterator_t> items_map_;
    size_t max_size_;
};

}   // namespace dsplib
