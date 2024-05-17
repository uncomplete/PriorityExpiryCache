/**
 * @author      : Eric Eastman (uncomplete3@gmail.com)
 * @file        : basic_pq
 * @created     : Saturday Nov 20, 2021 01:49:17 UTC
 */

#ifndef BASIC_PQ_HPP
#define BASIC_PQ_HPP

#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <list>
#include <queue>
#include <algorithm>

using namespace std;

typedef int    cval_t;
typedef string ckey_t;
typedef int    cprio_t;
typedef size_t ctime_t;

class NotFoundException: public exception
{
  virtual const char* what() const throw()
  {
    return "Item not found in cache";
  }
} NotFound;

template<typename T, class Container=std::vector<T>, class Compare=std::less<typename Container::value_type>>
class custom_priority_queue : public std::priority_queue<T, Container, Compare>
{
public:
  bool remove(const T& value) {
    auto it = std::find(this->c.begin(), this->c.end(), value);
    if (it != this->c.end()) {
      this->c.erase(it);
      std::make_heap(this->c.begin(), this->c.end(), this->comp);
      return true;
    } else {
      return false;
    }
  }
};

struct ExpireNode {
  ExpireNode(ctime_t exp, ckey_t k) : expire(exp), key(k) {};
  ctime_t expire;
  ckey_t key;
};

bool operator==(const ExpireNode& n1, const ExpireNode& n2) {
  return n1.expire==n2.expire && n1.key==n2.key;
}

bool operator>(const ExpireNode &n1, const ExpireNode &n2) {
  //return n1.expiry > n2.expiry;
  return n1.expire == n2.expire ?
         n1.key > n2.key :
         n1.expire > n2.expire;
}

struct Item {
  Item(ckey_t key, cval_t val, cprio_t prio, ctime_t exp) : 
      key(key), value(val), priority(prio), expire(exp) {};
  ckey_t key;
  cval_t value;
  cprio_t priority;
  ctime_t expire;
};

struct PriorityExpiryCache {
  PriorityExpiryCache(size_t maxItems) : _maxItems(maxItems) {
    cout << "Using Regular Priority Queue\n";
  }

  ctime_t _time;
  size_t _maxItems;

  custom_priority_queue<ExpireNode, vector<ExpireNode>, greater<ExpireNode>> exp_q;
  map<cprio_t, list<Item>> priority_map;
  unordered_map<ckey_t, list<Item>::iterator> key_map;

  bool empty() {
    return key_map.empty();
  }

  size_t size() {
    return key_map.size();
  }

  auto Keys() {
    vector<ckey_t> keys;
    for(const auto& it : key_map)
      keys.push_back(it.first);
    return keys;
  }

  void ResetTime() {
    _time=0;
  }

  void Sleep(ctime_t time) {
    _time+=time;
  }

  cval_t Get(std::string key) {
    auto ksearch = key_map.find(key);
    if (ksearch == key_map.end()) {
        return cval_t(); // should throw an exception
    }
    auto liter = ksearch->second;
    auto prio = liter->priority;
    auto psearch = priority_map.find(prio);
    auto &plist = psearch->second;
    plist.splice(plist.begin(), plist, liter);
    return liter->value;
  }

  void evictItem() {
    if( this->empty()) return;
    if( exp_q.top().expire < _time) {
        auto k = exp_q.top().key;
        exp_q.pop();
        auto liter = key_map[k];
        auto prio = liter->priority;
        key_map.erase(k);
        if( priority_map[prio].size() == 1) priority_map.erase(prio);
        else priority_map[prio].erase(liter);
    } else { // lowest priority
        auto prioIter = priority_map.begin();
        auto prio = prioIter->first;
        auto &plist = prioIter->second;
        auto k = plist.back().key;
        exp_q.remove(ExpireNode(plist.back().expire, k));
        key_map.erase(k);
        if( plist.size() == 1) priority_map.erase(prio);
        else plist.pop_back();
    }
  }

  void Set(ckey_t key, cval_t value, cprio_t priority, ctime_t expire) {
    if (key_map.count(key) > 0) throw NotFound;
    else if (key_map.size() == _maxItems) evictItem();
    priority_map[priority].emplace_front(key, value, priority, expire);
    key_map[key] = priority_map[priority].begin();
    exp_q.emplace(expire, key);
  }

  void SetMaxItems(size_t maxItems) {
    if( maxItems < _maxItems) {
      auto diff = _maxItems - maxItems;
      for(auto i=0;i<diff;i++) evictItem();
    }
    _maxItems = maxItems;
  }

};

PriorityExpiryCache* NewCache(int maxItems)
{
  return new PriorityExpiryCache(maxItems); // create and return PEC
}

#endif /* end of include guard BASIC_PQ_HPP */
