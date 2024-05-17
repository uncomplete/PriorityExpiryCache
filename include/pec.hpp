/**
 * @author      : Eric Eastman (uncomplete3@gmail.com)
 * @file        : pec
 * @created     : Saturday Nov 20, 2021 01:49:17 UTC
 */

#ifndef PEC_HPP
#define PEC_HPP

#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <list>
#include <queue>
#include <algorithm>

#include "basic_pq.hpp"

class NotFoundException: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Item not found in cache";
    }
} NotFound;

class ItemExpiredException: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Item has expired";
    }
} ItemExpired;

class AlreadyExistsException: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Item already exists in cache";
    }
} AlreadyExists;

template<typename Value, typename Key = std::size_t, typename Expiry = std::size_t,
        class Container = basic_priority_queue<std::tuple<Expiry, Key>>,
        typename Priority = std::size_t>
class PriorityExpiryCache {
public:
    typedef Container queue_type;
    typedef Value value_type;
    typedef Key key_type;
    typedef Priority priority_type;
    typedef Expiry expiry_type;
    typedef std::list<std::tuple<key_type, value_type, priority_type, expiry_type>> list_type;
    typedef std::map<priority_type, list_type> prio_map_type;
    typedef std::unordered_map<key_type, typename list_type::iterator> key_map_type;

    PriorityExpiryCache(size_t maxItems) : _maxItems(maxItems), _time(0) {
        std::cout << "PriorityExpiryCache( " << maxItems << " )\n";
    }

    bool empty() {
        return key_map.empty();
    }

    size_t size() {
        return key_map.size();
    }

    auto keys() {
        std::vector<key_type> keys;
        for(const auto& it : key_map)
            keys.push_back(it.first);
        return keys;
    }

    void resetTime() {
        _time=0;
    }

    expiry_type getTime() {
        return _time;
    }

    void sleep(expiry_type time) {
        _time+=time;
    }

    size_t capacity() {
        return _maxItems;
    }

    void clear() {
        key_map.clear();
        priority_map.clear();
        exp_q.clear();
    }

    void setMaxItems(size_t maxItems) {
        if( maxItems < _maxItems) {
            auto diff = _maxItems - maxItems;
            for(auto i=0;i<diff;i++) evictItem();
        }
        _maxItems = maxItems;
    }

    value_type get(key_type key) {
        auto ksearch = key_map.find(key);
        if (ksearch == key_map.end()) throw NotFoundException();
        auto liter = ksearch->second;
        const auto [k, value, priority, e] = *liter;
        auto psearch = priority_map.find(priority);
        auto &plist = psearch->second;
        plist.splice(plist.begin(), plist, liter);
        return value;
    }

    void evictItem() {
        if( this->empty()) return;
        const auto [expiry, key] = exp_q.top();
        if( expiry < _time) {
            exp_q.pop();
            auto liter = key_map[key];
            const auto [k, v, p, e] = *liter;
            key_map.erase(k);
            if( priority_map[p].size() == 1) priority_map.erase(p);
            else priority_map[p].erase(liter);
        } else { // lowest priority
            auto map_iter = priority_map.begin();
            auto &list = map_iter->second;
            const auto [k, v, p, e] = list.back();
            exp_q.remove_value(std::make_tuple(e, k));
            key_map.erase(k);
            if( list.size() == 1) priority_map.erase(p);
            else list.pop_back();
        }
    }

    void set(key_type key, value_type value, priority_type priority, expiry_type expire) {
        if (expire < _time) throw ItemExpiredException();
        else if (key_map.count(key) > 0) throw AlreadyExistsException();
        else if (key_map.size() == _maxItems) evictItem();
        priority_map[priority].emplace_front(std::make_tuple(key, value, priority, expire));
        key_map[key] = priority_map[priority].begin();
        exp_q.emplace(expire, key);
    }

private:
    expiry_type _time;
    size_t _maxItems;

    queue_type exp_q;
    prio_map_type priority_map;
    key_map_type key_map;
};

#endif /* end of include guard PEC_HPP */
