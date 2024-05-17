//
// Created by Eric Eastman on 5/16/24.
//

#define BOOST_TEST_MODULE MyTestSuite
#include <boost/test/included/unit_test.hpp>
#include <utility>

#include "indexed_pq.hpp"
#include "basic_pq.hpp"
#include "pec.hpp"

struct TestNode {
    int v1;
    std::string v2;

    TestNode() : v1(0) {};
    TestNode &operator=(const TestNode &rhs) = default;
    TestNode(int v1, std::string v2) : v1(v1), v2(std::move(v2)) {};

    bool operator<(const TestNode& rhs) const {
        return v1 == rhs.v1 ? v2 < rhs.v2 : v1 < rhs.v1;
    }

    bool operator>(const TestNode& rhs) const {
        return v1 == rhs.v1 ? v2 > rhs.v2 : v1 > rhs.v1;
    }

    bool operator==(const TestNode& rhs) const {
        return v1 == rhs.v1 && v2 == rhs.v2;
    }
};

// Custom specialization of std::hash can be injected in namespace std.
template<>
struct std::hash<TestNode>
{
    std::size_t operator()(const TestNode& s) const noexcept
    {
        std::size_t h1 = std::hash<int>{}(s.v1);
        std::size_t h2 = std::hash<std::string>{}(s.v2);
        return h1 ^ (h2 << 1); // or use boost::hash_combine
    }
};

typedef std::tuple<std::size_t, std::size_t> TestTuple;

template<>
struct std::hash<std::tuple<std::size_t, std::size_t>>
{
    std::size_t operator()(const std::tuple<std::size_t, std::size_t>& s) const
    {
        std::size_t h1 = std::hash<std::size_t>{}(std::get<0>(s));
        std::size_t h2 = std::hash<std::size_t>{}(std::get<1>(s));
        return h1 ^ (h2 << 1); // or use boost::hash_combine
    }
};


BOOST_AUTO_TEST_CASE(Indexed_TestCase0)
{
    std::cout << "Indexed_TestCase0\n";
    indexed_priority_queue<int, std::vector<int>, std::greater<>> ipq;
    ipq.push(1, 1);
    ipq.push(2, 2);
    ipq.push(3, 3);
    ipq.push(4, 4);
    ipq.push(5, 5);
    BOOST_CHECK(ipq.top() == 1);
    BOOST_CHECK(ipq.top_key() == 1);
    ipq.pop();
    BOOST_CHECK(ipq.top() == 2);
    BOOST_CHECK(ipq.top_key() == 2);
    ipq.pop();
    BOOST_CHECK(ipq.top() == 3);
    BOOST_CHECK(ipq.top_key() == 3);
    ipq.pop();
    BOOST_CHECK(ipq.top() == 4);
    BOOST_CHECK(ipq.top_key() == 4);
    ipq.pop();
    BOOST_CHECK(ipq.top() == 5);
    BOOST_CHECK(ipq.top_key() == 5);
    ipq.pop();
    BOOST_CHECK(ipq.top() == 0);
    BOOST_CHECK(ipq.top_key() == 0);
    BOOST_CHECK(ipq.size() == 0);
    BOOST_CHECK(ipq.empty() == true);
}

BOOST_AUTO_TEST_CASE(Indexed_TestCase1)
{
    std::cout << "Indexed_TestCase1\n";
    indexed_priority_queue<int, std::vector<int>> ipq;
    ipq.push(1, 1);
    ipq.push(2, 2);
    ipq.push(3, 3);
    ipq.push(4, 4);
    ipq.push(5, 5);
    ipq.remove_key(3);
    BOOST_CHECK(ipq.top() == 5);
    BOOST_CHECK(ipq.top_key() == 5);
    ipq.remove_key(1);
    BOOST_CHECK(ipq.top() == 5);
    BOOST_CHECK(ipq.top_key() == 5);
    ipq.remove_key(5);
    BOOST_CHECK(ipq.top() == 4);
    BOOST_CHECK(ipq.top_key() == 4);
    ipq.remove_key(2);
    BOOST_CHECK(ipq.top() == 4);
    BOOST_CHECK(ipq.top_key() == 4);
    ipq.remove_key(4);
    BOOST_CHECK(ipq.top() == 0);
    BOOST_CHECK(ipq.top_key() == 0);
    BOOST_CHECK(ipq.size() == 0);
    BOOST_CHECK(ipq.empty() == true);
}

BOOST_AUTO_TEST_CASE(Indexed_TestCase2)
{
    std::cout << "Indexed_TestCase2\n";
    indexed_priority_queue<TestNode, std::vector<TestNode>, std::greater<>> ipq;
    ipq.push(1, {1, "1"});
    ipq.push(2, {2, "2"});
    ipq.push(3, {3, "3"});
    ipq.push(4, {4, "4"});
    ipq.push(5, {5, "5"});
    ipq.remove_key(3);
    BOOST_CHECK(ipq.top() == TestNode(1, "1"));
    ipq.remove_key(1);
    BOOST_CHECK(ipq.top() == TestNode(2, "2"));
    ipq.remove_key(5);
    BOOST_CHECK(ipq.top() == TestNode(2, "2"));
    ipq.remove_key(2);
    BOOST_CHECK(ipq.top() == TestNode(4, "4"));
    ipq.remove_key(4);
    BOOST_CHECK(ipq.top() == TestNode(0, ""));
    BOOST_CHECK(ipq.size() == 0);
    BOOST_CHECK(ipq.empty() == true);
}

BOOST_AUTO_TEST_CASE(Basic_TestCase0)
{
    std::cout << "Basic_TestCase0\n";
    basic_priority_queue<int, std::vector<int>, std::greater<>> ipq;
    ipq.push(1);
    ipq.push(2);
    ipq.push(3);
    ipq.push(4);
    ipq.push(5);
    BOOST_CHECK(ipq.top() == 1);
    ipq.pop();
    BOOST_CHECK(ipq.top() == 2);
    ipq.pop();
    BOOST_CHECK(ipq.top() == 3);
    ipq.pop();
    BOOST_CHECK(ipq.top() == 4);
    ipq.pop();
    BOOST_CHECK(ipq.top() == 5);
    ipq.pop();
    BOOST_CHECK(ipq.size() == 0);
    BOOST_CHECK(ipq.empty() == true);
}

BOOST_AUTO_TEST_CASE(Basic_TestCase1)
{
    std::cout << "Basic_TestCase1\n";
    basic_priority_queue<int> ipq;
    ipq.push(1);
    ipq.push(2);
    ipq.push(3);
    ipq.push(4);
    ipq.push(5);
    ipq.remove_value(3);
    BOOST_CHECK(ipq.top() == 5);
    ipq.remove_value(1);
    BOOST_CHECK(ipq.top() == 5);
    ipq.remove_value(5);
    BOOST_CHECK(ipq.top() == 4);
    ipq.remove_value(2);
    BOOST_CHECK(ipq.top() == 4);
    ipq.remove_value(4);
    BOOST_CHECK(ipq.size() == 0);
    BOOST_CHECK(ipq.empty() == true);
}

BOOST_AUTO_TEST_CASE(Cache_TestCase0)
{
    std::cout << "Cache_TestCase0\n";
    PriorityExpiryCache<std::string> c(10);
    BOOST_CHECK(c.capacity() == 10);
    BOOST_CHECK(c.empty() == true);
    BOOST_CHECK(c.size() == 0);
    c.setMaxItems(5);
    BOOST_CHECK(c.capacity() == 5);
    c.set(1, "1", 1, 1);
    c.set(2, "2", 2, 2);
    c.set(3, "3", 3, 3);
    c.set(4, "4", 4, 4);
    c.set(5, "5", 5, 5);
    BOOST_CHECK(c.size() == 5);
    BOOST_CHECK_THROW(c.set(5, "6", 6, 6), AlreadyExistsException);
    c.set(6, "6", 6, 6);
    BOOST_CHECK(c.size() == 5);
    BOOST_CHECK_THROW(c.get(1), NotFoundException);
    BOOST_CHECK(c.get(2) == "2");
    BOOST_CHECK(c.get(3) == "3");
    BOOST_CHECK(c.get(4) == "4");
    BOOST_CHECK(c.get(5) == "5");
    BOOST_CHECK(c.get(6) == "6");
}

BOOST_AUTO_TEST_CASE(Cache_TestCase1)
{
    std::cout << "Cache_TestCase0\n";
    PriorityExpiryCache<std::string, std::size_t, std::size_t, indexed_priority_queue<TestTuple>> c(10);
    BOOST_CHECK(c.capacity() == 10);
    BOOST_CHECK(c.empty() == true);
    BOOST_CHECK(c.size() == 0);
    c.setMaxItems(5);
    BOOST_CHECK(c.capacity() == 5);
    c.set(1, "1", 1, 1);
    c.set(2, "2", 2, 2);
    c.set(3, "3", 3, 3);
    c.set(4, "4", 4, 4);
    c.set(5, "5", 5, 5);
    BOOST_CHECK(c.size() == 5);
    BOOST_CHECK_THROW(c.set(5, "6", 6, 6), AlreadyExistsException);
    c.set(6, "6", 6, 6);
    BOOST_CHECK(c.size() == 5);
    BOOST_CHECK_THROW(c.get(1), NotFoundException);
    BOOST_CHECK(c.get(2) == "2");
    BOOST_CHECK(c.get(3) == "3");
    BOOST_CHECK(c.get(4) == "4");
    BOOST_CHECK(c.get(5) == "5");
    BOOST_CHECK(c.get(6) == "6");
}