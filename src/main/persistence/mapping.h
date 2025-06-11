#ifndef MAPPING_H
#define MAPPING_H

#include <string_view>

template<typename MappedType>
constexpr std::string_view table_name() = delete;

template<typename MappedType>
constexpr auto to_tuple(const MappedType& c) = delete;

template<typename MappedType, typename Tuple>
constexpr MappedType from_tuple(const Tuple&) = delete;

template<typename T>
using tuple_type_t = decltype(to_tuple(std::declval<T>()));
#endif
//MAPPING_H
