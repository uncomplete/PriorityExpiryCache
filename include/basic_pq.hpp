/**
 * @author      : Eric Eastman (uncomplete3@gmail.com)
 * @file        : basic_pq
 * @created     : Saturday Nov 20, 2021 01:49:17 UTC
 */

#ifndef BASIC_PQ_HPP
#define BASIC_PQ_HPP

#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>

#include "utils.hpp"

template<Hashable T, class Container=std::vector<T>, class Compare=std::less<typename Container::value_type>>
class basic_priority_queue : public std::priority_queue<T, Container, Compare>
{
    public:
    bool remove_value(const T& value) {
        auto it = std::find(this->c.begin(), this->c.end(), value);
        if (it != this->c.end()) {
          this->c.erase(it);
          std::make_heap(this->c.begin(), this->c.end(), this->comp);
          return true;
        } else {
          return false;
        }
    }

    bool remove_key(const std::size_t key) {
        auto it = _keys.find(key);
        if (it != _keys.end()) {
            return remove_value(it->second);
        } else {
            return false;
        }
    }

    private:
    std::unordered_map<std::size_t, T> _keys;
};


#endif /* end of include guard BASIC_PQ_HPP */
