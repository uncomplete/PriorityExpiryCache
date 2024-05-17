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

// Define a concept that PriorityExpiryCache requires

template<class K, class T>
class index_min_pq {
    public:
        index_min_pq() : n(0) {
            pq.push_back(0);
            keys[K()] = 0;
            _keys[0] = K();
        };

        T top() { return n>0 ? pq[1] : T(); };

        K top_key() { return n>0 ? _keys[1] : K(); };

        void pop() {
            if(n==0) return;
            exch(1, n--);
            sink(1);
            pq.pop_back();
            keys.erase(_keys[n+1]);
            _keys.erase(n+1);
        }

        void push(K s, T exp) {
            pq.push_back(exp);
            keys.emplace(s, ++n);
            _keys.emplace(n, s);
            swim(n);
        }

        void remove(K s) {
            auto ind = keys[s];
            exch(ind, n--);
            swim(ind);
            sink(ind);
            pq.pop_back();
            keys.erase(s);
            _keys.erase(n+1);
        }

        void print() {
            cout << n << " ";
            for(int i=1;i<=n;i++)
                cout << pq[i] << " ";
            cout << "\n";
        }

        bool empty() { return n == 0; }

    private:
        unordered_map<K, size_t> keys;
        unordered_map<size_t, K> _keys;
        vector<T> pq;
        size_t n;

        bool greater(size_t i, size_t j) {
            return pq[i] > pq[j];
        }

        void exch(size_t i, size_t j) {
            K& si = _keys[i];
            K& sj = _keys[j];
            std::swap(keys[si], keys[sj]);
            std::swap(si, sj);
            std::swap(pq[i], pq[j]);
        }

        void swim(size_t k) {
            while (k > 1 && greater(k/2, k)) {
                exch(k, k/2);
                k = k/2;
            }
        }

        void sink(size_t k) {
            while (2*k <= n) {
                size_t j = 2*k;
                if (j < n && greater(j, j+1)) j++;
                if (!greater(k, j)) break;
                exch(k, j);
                k = j;
            }
        }
};

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
    cout << "Using Indexed Priority Queue\n";
  }

  ctime_t _time;
  size_t _maxItems;

  index_min_pq<ckey_t, ctime_t> exp_q;
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
    if( exp_q.top() < _time) {
        auto k = exp_q.top_key();
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
        exp_q.remove(k);
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
    exp_q.push(key, expire);
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
