#ifndef DISCRETE_TYPES_H
#define DISCRETE_TYPES_H

#include <type_traits>
#include <variant>
#include <tuple>

template < typename ... >
struct type_list {};

template<typename List, typename T>
struct contains_type;
template<typename T>
struct contains_type<type_list<>, T> : std::false_type {};

template<typename Head, typename... Tail, typename T>
struct contains_type<type_list<Head, Tail...>, T>
: std::conditional_t<std::is_same_v<Head, T>, std::true_type, contains_type<type_list<Tail...>, T>> {};

template<typename T, typename TL>
struct add_type;

template<typename T, typename... Ts>
struct add_type<T, type_list<Ts...>> {
    using type = type_list<Ts..., T>;
};

template<typename Tuple>
struct rebind_tuple;

template<typename... Ts>
struct rebind_tuple<std::tuple<Ts...>> {
    template<template<typename...> typename Template>
    using to = Template<Ts...>;

    template<typename Func, typename... Args>
    static decltype(auto) call(Func&& f, Args&&... args) {
        return std::forward<Func>(f).template operator()<Ts...>(std::forward<Args>(args)...);
    }
};

template<typename List, typename T>
concept type_in_list = contains_type<List, T>::value;

template <typename T>
void hash_combine(std::size_t& seed, const T& value) {
    seed ^= std::hash<T>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <typename T, typename U>
constexpr std::size_t offset_of(U T::*member) {
    return reinterpret_cast<std::size_t>(&((T*)0->*member));
}

#endif //DISCRETE_TYPES_H
