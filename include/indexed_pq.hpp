/**
 * @author      : Eric Eastman (uncomplete3@gmail.com)
 * @file        : basic_pq
 * @created     : Saturday Nov 20, 2021 01:49:17 UTC
 */

#ifndef INDEXED_PQ_HPP
#define INDEXED_PQ_HPP

#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <list>
#include <queue>
#include <algorithm>

template<typename T,
        class Container = std::vector<T>,
        class Compare = std::less<typename Container::value_type>,
        std::unsigned_integral K = std::size_t,
        class Hash = std::hash<T>>
class indexed_priority_queue {
    public:
    typedef Container   container_type;
    typedef Compare     value_compare;
    typedef Hash        key_hasher;

    indexed_priority_queue() : n(0) {
        T tmpValue = T();
        K tmpKey = hash(tmpValue);
        pq.push_back(tmpValue);
        keys.emplace(tmpKey, n);
        _keys.emplace(n, tmpKey);
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

    K push(T exp) {
        K s = hash(exp);
        pq.push_back(exp);
        keys.emplace(s, ++n);
        _keys.emplace(n, s);
        swim(n);
        return s;
    }

    template <typename... Args>
    K emplace(Args&&... args) {
        pq.emplace_back(std::forward<Args>(args)...);
        K s = hash(pq.back());
        keys.emplace(s, ++n);
        _keys.emplace(n, s);
        swim(n);
        return s;
    }

    bool remove_key(K s) {
        auto search = keys.find(s);
        if (search == keys.end()) return false;
        size_t ind = search->second;
        exch(ind, n--);
        swim(ind);
        sink(ind);
        pq.pop_back();
        keys.erase(s);
        _keys.erase(n+1);
        return true;
    }

    bool remove_value(T exp) {
        K s = hash(exp);
        auto search = keys.find(s);
        if (search == keys.end()) return false;
        size_t ind = search->second;
        exch(ind, n--);
        swim(ind);
        sink(ind);
        pq.pop_back();
        keys.erase(s);
        _keys.erase(n+1);
        return true;
    }


    void print() {
        std::cout << n << " [ ";
        for (const auto& e : pq)
            std::cout << e << " ";
        std::cout << "]\n";
    }

    bool empty() { return n == 0; }

    size_t size() { return n; }

    private:
    std::unordered_map<K, size_t> keys;
    std::unordered_map<size_t, K> _keys;
    container_type pq;
    value_compare comp = value_compare{};
    key_hasher hash = key_hasher{};
    size_t n;

    bool greater(size_t i, size_t j) {
        return !comp(pq[j], pq[i]);
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

#endif /* end of include guard INDEXED_PQ_HPP */
