#ifndef DISCRETE_TYPES_H
#define DISCRETE_TYPES_H

#include <type_traits>
#include <variant>

template < typename ... Types >
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

template<typename List, typename T>
concept type_in_list = contains_type<List, T>::value;

template<typename... Types>
std::variant<Types...> as_variant(type_list<Types...>);

template <typename T>
inline void hash_combine(std::size_t& seed, const T& value) {
    seed ^= std::hash<T>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

#endif //DISCRETE_TYPES_H
