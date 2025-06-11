#ifndef DISCRETE_DATABASE_H
#define DISCRETE_DATABASE_H

#include "sqlite3.h"
#include "util/types.h"
#include "util/fixed_string.h"
#include "event/EventManager.h"
#include "log.h"
#include "config.h"

#include <string>
#include <vector>
#include <optional>
#include <string_view>

using supported_sqlite_types = type_list<sqlite3_int64, double, std::string>;
using namespace std::literals::string_view_literals;

template<typename Type>
concept sqlite_type =
    contains_type<supported_sqlite_types, Type>::value ||
    (
        requires { typename Type::value_type; } && // does the type have a value_type (is it a std container?)
        contains_type<supported_sqlite_types, typename Type::value_type>::value && // is the contained value a supported_sqlite_types
        std::same_as<Type, std::optional<typename Type::value_type>> // is the container type an optional with a support value type?
    );

template<typename T>
void bindParameter(sqlite3_stmt* stmt, int index, const T& value);

template<>
inline void bindParameter<int>(sqlite3_stmt* stmt, int index, const int& value) {
    sqlite3_bind_int(stmt, index, value);
}

template<>
inline void bindParameter<double>(sqlite3_stmt* stmt, int index, const double& value) {
    sqlite3_bind_double(stmt, index, value);
}

template<>
inline void bindParameter<std::string>(sqlite3_stmt* stmt, int index, const std::string& value) {
    sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
}

template<>
inline void bindParameter<sqlite3_int64>(sqlite3_stmt* stmt, int index, const sqlite3_int64& value) {
    sqlite3_bind_int64(stmt, index, value);
}


template <fixed_string queryStatement, sqlite_type ...TupleTypes, sqlite_type ... BindingTypes>
auto executeQuery(
    sqlite3* database,
    BindingTypes... bindings
){

    std::tuple<BindingTypes...> bindingTuple{bindings...};
    sqlite3_stmt* statement;

    if (sqlite3_prepare_v2(database, queryStatement.view().data(),-1,&statement,nullptr) != SQLITE_OK) {
        log("Failed to prepare statement:" + std::string(sqlite3_errmsg(database)), spdlog::level::err);
        throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(database)));
    }

    [&]<std::size_t... IndexSequence>(std::index_sequence<IndexSequence...>) {
        (bindParameter(std::get<IndexSequence>(bindingTuple), IndexSequence + 1, std::get<IndexSequence>(bindingTuple)), ...);
    }(std::index_sequence_for<BindingTypes...>{});

    if constexpr (
    []{
        constexpr std::string_view v = queryStatement.view();
        // TODO equalsIgnoreCase this.
        return v.find("SELECT") == std::string_view::npos;
    }()
    ) {
        sqlite3_step(statement);
        sqlite3_finalize(statement);
    } else
    {

        std::vector<std::tuple<TupleTypes...>> results;

        while (sqlite3_step(statement) == SQLITE_ROW) {
            try {
                results.emplace_back(extractRow<TupleTypes...>(statement));
            } catch (const std::exception& e) {
                sqlite3_finalize(statement);
                log("Failed to extract row: " + std::string(sqlite3_errmsg(database)), spdlog::level::err);
                throw std::runtime_error("Failed to extract row: " + std::string(e.what()));
            }
        }

        sqlite3_finalize(statement);
        return results;

    }

}

template<sqlite_type ...TupleTypes>
std::vector<std::tuple<TupleTypes...>> getAllRecords(
    sqlite3* database,
    const std::string& table
){
    const std::string sql = "SELECT * FROM " + table + ";";
    sqlite3_stmt* statement;

    if (sqlite3_prepare_v2(database, sql.c_str(), -1, &statement, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare SQL statement: " + std::string(sqlite3_errmsg(database)));
    }

    std::vector<std::tuple<TupleTypes...>> results;

    while (sqlite3_step(statement) == SQLITE_ROW) {
        try {
            results.emplace_back(extractRow<TupleTypes...>(statement));
        } catch (const std::exception& e) {
            sqlite3_finalize(statement);
            throw std::runtime_error("Error extracting row: " + std::string(e.what()));
        }
    }

    sqlite3_finalize(statement);

    return results;
}

template<sqlite_type ...TupleTypes>
std::optional<std::tuple<TupleTypes...>> getRecord(
    sqlite3* database,
    const std::string& table,
    const sqlite3_int64 & id
){
    std::string sql = "SELECT * FROM " + table + " WHERE id = ?;";
    sqlite3_stmt* statement;

    sqlite3_prepare_v2(database, sql.c_str(), -1, &statement, nullptr);
    bindParameter(statement, 1, id);

    if (sqlite3_step(statement) != SQLITE_ROW) {
        sqlite3_finalize(statement);
        {};
    }

    auto result = extractRow<TupleTypes...>(statement);
    sqlite3_finalize(statement);
    return result;
}

template<sqlite_type SqliteType, typename ...TupleTypes>
std::optional<std::tuple<TupleTypes...>> getRecord(
    sqlite3* database,
    const std::string& table,
    const std::string& indexName,
    const SqliteType& value
){
    std::string sql = "SELECT * FROM " + table + " WHERE " + indexName + " = ?;";
    sqlite3_stmt* statement;

    if (sqlite3_prepare_v2(database, sql.c_str(), -1, &statement, nullptr) != SQLITE_OK) {
    }

    bindParameter(statement, 1, value);

    if (sqlite3_step(statement) != SQLITE_ROW) {
        sqlite3_finalize(statement);
        return {};
    }

    auto result = extractRow<TupleTypes...>(statement);
    sqlite3_finalize(statement);
    return result;
}

template <typename MappedType, sqlite_type ValueType>
std::optional<MappedType> getRecord(sqlite3* database, const std::string& indexName, const ValueType& value)
{

    return rebind_tuple<tuple_type_t<MappedType>>::call([&]<typename... TupleTypes>(
    ) {
        return getRecord<ValueType, TupleTypes...>(database, table_name<MappedType>().data(), indexName,  value);
    })
    .transform([](const auto& record){
        return from_tuple<MappedType>(record);
    });

}

template <typename MappedType>
std::optional<MappedType> getRecord(sqlite3* database, const sqlite3_int64& id)
{

    return rebind_tuple<tuple_type_t<MappedType>>::call([&]<typename... Ts>(
    ) {
        return getRecord< Ts...>(database, std::string{table_name<MappedType>().data()}, id);
    })
    .transform([](const auto& record){
        return from_tuple<MappedType>(record);
    });

}

template<sqlite_type T>
static T convertSQLiteColumn(sqlite3_stmt* preparedStatement, int index) = delete;

// Handle optional columns by checking for a value and then calling the specialised template for the value type, if present
template<sqlite_type T>
requires std::same_as<T, std::optional<typename T::value_type>>
static std::optional<typename T::value_type> convertSQLiteColumn(sqlite3_stmt* preparedStatement, int index){
    if (sqlite3_column_type(preparedStatement, index) != SQLITE_NULL)
        return {
            convertSQLiteColumn<typename T::value_type>(preparedStatement, index)
        };
    return std::nullopt;
}

template<>
inline sqlite3_int64 convertSQLiteColumn<sqlite3_int64>(sqlite3_stmt* preparedStatement, int index) {
    return sqlite3_column_int64(preparedStatement, index);
}

template<>
inline double convertSQLiteColumn<double>(sqlite3_stmt* preparedStatement, int index) {
    return sqlite3_column_double(preparedStatement, index);
}

template<>
inline std::string convertSQLiteColumn<std::string>(sqlite3_stmt* preparedStatement, int index) {
    return reinterpret_cast<const char*>(sqlite3_column_text(preparedStatement, index));
}

template<sqlite_type... TupleTypes, std::size_t... Indices>
static std::tuple<TupleTypes...> extractRow(sqlite3_stmt* preparedStatement, std::index_sequence<Indices...>) {
    return {
        (convertSQLiteColumn<TupleTypes>(preparedStatement, Indices))...
    };
}

template<sqlite_type... TupleTypes>
static std::tuple<TupleTypes...> extractRow(sqlite3_stmt* preparedStatement) {
    if (sizeof...(TupleTypes) != sqlite3_column_count(preparedStatement)) {
        throw std::runtime_error("Mismatch between tuple size and column count");
    }

    return extractRow<TupleTypes...>(preparedStatement, std::index_sequence_for<TupleTypes...>{});
}

struct TableUpdate{
    int operation;
    const sqlite3_int64 rowId;
};

struct ConfigUpdate: TableUpdate{};
struct InputUpdate: TableUpdate{};

inline void dbEventCallback(
    void* data,
    int operation,
    const char* database,
    const char* table,
    sqlite3_int64 rowId
){

    auto eventManager{ *static_cast<std::shared_ptr<EventManager>*>(data)};

    if(!strcmp(table, "config")) {
        eventManager->publishEvent(
                ConfigUpdate{
                        operation, rowId
                }
        );
    }

    if(!strcmp(table, "input")) {
        eventManager->publishEvent(
                InputUpdate{
                        operation, rowId
                }
        );
    }

}

#endif //DISCRETE_DATABASE_H