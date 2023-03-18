#pragma once

#include <list>
#include <unordered_map>
#include <cassert>

template<class Key, class T>
class LRUCache
{
public:
    explicit LRUCache(int size)
      : cache_size_(size){};

    void put(const Key& key, const T& val) {
        auto it = item_map_.find(key);
        if (it != item_map_.end()) {
            item_list_.erase(it->second);
            item_map_.erase(it);
        }
        item_list_.push_front(std::make_pair(key, val));
        item_map_.insert(std::make_pair(key, item_list_.begin()));
        clean();
    };

    bool exist(const Key& key) const {
        return (item_map_.count(key) > 0);
    };

    T get(const Key& key) {
        assert(exist(key));
        auto it = item_map_.find(key);
        item_list_.splice(item_list_.begin(), item_list_, it->second);
        return it->second->second;
    };

private:
    void clean() {
        while (item_map_.size() > cache_size_) {
            auto last_it = item_list_.end();
            last_it--;
            item_map_.erase(last_it->first);
            item_list_.pop_back();
        }
    };

    std::list<std::pair<Key, T>> item_list_;
    std::unordered_map<Key, decltype(item_list_.begin())> item_map_;
    size_t cache_size_;
};