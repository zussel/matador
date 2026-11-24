#ifndef MATADOR_META_TABLE_MACRO_HPP
#define MATADOR_META_TABLE_MACRO_HPP

#include "matador/utils/macro_map.hpp"

#include "matador/query/model/table.hpp"

#include <utility>

#define FIELD(FIELD_NAME) const matador::query::column& FIELD_NAME;
#define INIT_FIELD(FIELD_NAME) , FIELD_NAME(*column_by_name(*this, #FIELD_NAME))
#define FIELD_STRING(FIELD_NAME) column::make_plain(#FIELD_NAME),

#define TYPED_FIELD(FIELD_NAME, FIELD_TYPE) const matador::query::column& FIELD_NAME;
#define INIT_TYPED_FIELD(FIELD_NAME, FIELD_TYPE) , FIELD_NAME(*column_by_name(*this, #FIELD_NAME))
#define TYPED_FIELD_COLUMN(FIELD_NAME, FIELD_TYPE) matador::query::column{nullptr, #FIELD_NAME, FIELD_TYPE}

#define META_TABLE(TABLE_NAME, VARIABLE_NAME, ...) \
namespace matador::query::meta { \
namespace internal { \
class TABLE_NAME##_table : public typed_query_table<TABLE_NAME##_table> { \
public: \
TABLE_NAME##_table() \
: typed_query_table<TABLE_NAME##_table>(#TABLE_NAME, {MAP(FIELD_STRING, __VA_ARGS__)}) \
MAP(INIT_FIELD, __VA_ARGS__) \
{} \
TABLE_NAME##_table(const TABLE_NAME##_table& other) \
: typed_query_table<TABLE_NAME##_table>(other) \
MAP(INIT_FIELD, __VA_ARGS__) \
{} \
TABLE_NAME##_table(TABLE_NAME##_table&& other) noexcept \
: typed_query_table<TABLE_NAME##_table>(std::move(other)) \
MAP(INIT_FIELD, __VA_ARGS__) \
{} \
TABLE_NAME##_table(matador::query::table source) \
: typed_query_table<TABLE_NAME##_table>(std::move(source)) \
MAP(INIT_FIELD, __VA_ARGS__) \
{} \
TABLE_NAME##_table& operator=(const TABLE_NAME##_table&) = delete; \
TABLE_NAME##_table& operator=(TABLE_NAME##_table&&) = delete; \
MAP(FIELD, __VA_ARGS__) \
}; } \
inline const internal::TABLE_NAME##_table VARIABLE_NAME; \
}

#define META_TYPED_TABLE(TABLE_NAME, VARIABLE_NAME, ...) \
namespace matador::query::meta { \
namespace internal { \
class TABLE_NAME##_table : public typed_query_table<TABLE_NAME##_table> { \
public: \
TABLE_NAME##_table() \
: TABLE_NAME##_table(#TABLE_NAME) \
{} \
TABLE_NAME##_table(const std::string& alias) \
: typed_query_table<TABLE_NAME##_table>(#TABLE_NAME, alias, {MAP_PAIR_LIST(TYPED_FIELD_COLUMN, __VA_ARGS__)}) \
MAP_PAIR(INIT_TYPED_FIELD, __VA_ARGS__) \
{} \
TABLE_NAME##_table(const TABLE_NAME##_table& other) \
: typed_query_table<TABLE_NAME##_table>(other) \
MAP_PAIR(INIT_TYPED_FIELD, __VA_ARGS__) \
{} \
TABLE_NAME##_table(TABLE_NAME##_table&& other) noexcept \
: typed_query_table<TABLE_NAME##_table>(std::move(other)) \
MAP_PAIR(INIT_TYPED_FIELD, __VA_ARGS__) \
{} \
TABLE_NAME##_table& operator=(const TABLE_NAME##_table&) = delete; \
TABLE_NAME##_table& operator=(TABLE_NAME##_table&&) = delete; \
MAP_PAIR(TYPED_FIELD, __VA_ARGS__) \
}; } \
inline const internal::TABLE_NAME##_table VARIABLE_NAME; \
}

#endif //MATADOR_META_TABLE_MACRO_HPP