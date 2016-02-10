#pragma once

#include <type_traits>
#include <utility>

// Nice little utility that c++14's auto lambdas makes possible
template <bool Cond, class F1, class F2, class... Args>
std::enable_if_t<Cond> call_if_else(const F1& f1, const F2& f2, Args&&... args) {
    f1(std::forward<Args>(args)...);
}

template <bool Cond, class F1, class F2, class... Args>
std::enable_if_t<!Cond> call_if_else(const F1& f1, const F2& f2, Args&&... args) {
    f2(std::forward<Args>(args)...);
}

// Nice little utility that c++14's auto lambdas makes possible
template <bool Cond, class F1, class... Args>
std::enable_if_t<Cond> call_if(const F1& f1, Args&&... args) {
    f1(std::forward<Args>(args)...);
}

template <bool Cond, class F1, class... Args>
std::enable_if_t<!Cond> call_if(const F1&, Args&&... args) {}

template <class>
struct reflect;

template <class T>
struct is_reflection_enabled : public std::is_base_of<reflect<T>, T> {};
