#ifndef DISCRETE_DATABASE_H
#define DISCRETE_DATABASE_H

#include "sqlite3.h"
#include "util/types.h"
#include "event/EventManager.h"
#include <string>
#include <vector>
#include <cinttypes>
#include <optional>

using supported_sqlite_types = type_list<sqlite3_int64, double, std::string>;

template<typename Type>
concept sqlite_type =
    contains_type<supported_sqlite_types, Type>::value ||
    (
        requires { typename Type::value_type; } && // does the type have a value_type (is it a std container?)
        contains_type<supported_sqlite_types, typename Type::value_type>::value && // is the contained value a supported_sqlite_types
        std::same_as<Type, std::optional<typename Type::value_type>> // is the container type an optional with a support value type?
    );

template<sqlite_type ...TupleTypes>
std::optional<std::tuple<TupleTypes...>> getRecord(
    sqlite3* database,
    const std::string& table,
    const sqlite3_int64 & id
){
    std::string sql = "SELECT * FROM " + table + " WHERE id = ?;";
    sqlite3_stmt* statement;

    sqlite3_prepare_v2(database, sql.c_str(), -1, &statement, nullptr);
    sqlite3_bind_int64(statement, 1, id);

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

    if constexpr (std::is_same_v<SqliteType, int>) {
        sqlite3_bind_int(statement, 1, value);
    } else if constexpr (std::is_same_v<SqliteType, double>) {
        sqlite3_bind_double(statement, 1, value);
    } else if constexpr (std::is_same_v<SqliteType, std::string>) {
        sqlite3_bind_text(statement, 1, value.c_str(), -1, SQLITE_TRANSIENT);
    }

    if (sqlite3_step(statement) != SQLITE_ROW) {
        sqlite3_finalize(statement);
        return {};
    }

    auto result = extractRow<TupleTypes...>(statement);
    sqlite3_finalize(statement);
    return result;
}

template<typename ...TupleTypes>
std::optional<std::vector<std::tuple<TupleTypes...>>> executeQuery(
    sqlite3* database,
    const std::string& queryStatement
){
    sqlite3_stmt* statement;

    sqlite3_prepare_v2(database, queryStatement.c_str(), -1, &statement, nullptr);


    if (sqlite3_step(statement) != SQLITE_ROW) {
        sqlite3_finalize(statement);
        return {};
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
std::vector<std::tuple<TupleTypes...>> getAllRecords(
    sqlite3* database,
    const std::string& table
){
    std::string sql = "SELECT * FROM " + table + ";";
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

template<sqlite_type T>
static T convertSQLiteColumn(sqlite3_stmt* preparedStatement, int index) = delete;

// Handle optional columns by checking for a value and then calling the specialised template for the value type, if present
template<sqlite_type T>
requires std::same_as<T, std::optional<typename T::value_type>>
static std::optional<typename T::value_type> convertSQLiteColumn(sqlite3_stmt* preparedStatement, int index){
    int type = sqlite3_column_type(preparedStatement, index);
    if (type != SQLITE_NULL)
        return {
            convertSQLiteColumn<typename T::value_type>(preparedStatement, index)
        };
    return std::nullopt;
}

template<>
sqlite3_int64 convertSQLiteColumn<sqlite3_int64>(sqlite3_stmt* preparedStatement, int index) {
    return sqlite3_column_int64(preparedStatement, index);
}

template<>
double convertSQLiteColumn<double>(sqlite3_stmt* preparedStatement, int index) {
    return sqlite3_column_double(preparedStatement, index);
}

template<>
std::string convertSQLiteColumn<std::string>(sqlite3_stmt* preparedStatement, int index) {
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