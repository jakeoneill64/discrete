#ifndef CONFIG_H
#define CONFIG_H

#include "sqlite3.h"
#include "mapping.h"
#include <string>
#include <string_view>

struct ConfigEntry
{
    sqlite3_int64 id;
    std::string key;
    std::string value;
};

template<>
constexpr std::string_view table_name<ConfigEntry>() {
    return "config";
}

template<>
constexpr auto to_tuple(const ConfigEntry& config) {
    return std::make_tuple(config.id, config.key, config.value);
}

template<>
constexpr ConfigEntry from_tuple(const std::tuple<sqlite3_int64, std::string, std::string>& configTuple) {
    return { std::get<0>(configTuple), std::get<1>(configTuple), std::get<2>(configTuple) };
}


#endif //CONFIG_H
