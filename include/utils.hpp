/**
 * @author      : Eric Eastman (uncomplete3@gmail.com)
 * @file        : utils
 * @created     : Saturday Nov 20, 2021 01:49:17 UTC
 */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstddef>
#include <concepts>
#include <functional>

// Declaration of the concept "Hashable", which is satisfied by any type 'T'
// such that for values 'a' of type 'T', the expression std::hash<T>{}(a)
// compiles and its result is convertible to std::size_t
template<typename T>
concept Hashable = requires(T a)
{
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

#endif /* end of include guard UTILS_HPP */
