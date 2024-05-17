/**
 * @author      : Eric Eastman (uncomplete3@gmail.com)
 * @file        : main
 * @created     : Monday Nov 15, 2021 19:23:28 UTC
 */

#include <iostream>
#include <algorithm>
#include <iterator>
#include <list>
#include <unordered_map>
#include <chrono>
#include <ctime>
#include <unistd.h>

#include "indexed_pq.hpp"
#include "basic_pq.hpp"

using namespace std;

void printKeys(PriorityExpiryCache& c) {
    for(const auto& s : c->Keys())
        cout << s << " ";
    cout << "\n";
}

vector<int> ivec = {5, 15, 1, 5, 5};
vector<size_t> zvec = {100, 3, 15, 150, 100};

std::string gen_random(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
}

void test2(PriorityExpiryCache& c, size_t n, size_t cn) {
    auto start = chrono::high_resolution_clock::now();

    c->ResetTime();
    c->SetMaxItems(cn);
    for(size_t i=0; i<n; i++) {
        c->SetMaxItems(cn);
        for(size_t j=0; j<cn; j++) {
            c->Set(gen_random(12), 6, ivec[j%ivec.size()], zvec[j%zvec.size()]);
        }
        for(size_t b=c->size(); b>0; b--) {
            c->SetMaxItems(b);
        }
        c->SetMaxItems(0);
    }

    auto end = chrono::high_resolution_clock::now();
    auto int_s = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << cn << "  " << int_s.count() << "\n";
}

int main()
{
    srand((unsigned)time(NULL) * getpid());
    auto c = NewCache(5);
    test2(c, 10, 1000);
    test2(c, 10, 2000);
    test2(c, 10, 3000);
    test2(c, 10, 4000);
    test2(c, 10, 5000);
    test2(c, 10, 6000);
    test2(c, 10, 7000);
    test2(c, 10, 8000);
    test2(c, 10, 9000);
    test2(c, 10, 10000);

    //test2(c, 10, 1000000);
    //test1(c, 1000);
    //test1(c, 10000);
    //test1(c, 100000);
    //test1(c, 1000000);

    delete c;
    return 0;
}
