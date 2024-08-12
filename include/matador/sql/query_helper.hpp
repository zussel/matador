#ifndef QUERY_QUERY_HELPER_HPP
#define QUERY_QUERY_HELPER_HPP

#include "matador/utils/macro_map.hpp"

#include "matador/sql/table.hpp"
#include "matador/sql/column.hpp"

#include <string>
#include <ostream>

#define FIELD(x) const sql::column x{*this, #x, ""};

#define QUERY_HELPER(C, ...) \
namespace matador::qh {     \
namespace internal {                      \
struct C##_query : sql::table {           \
C##_query() : table(#C) {}                            \
MAP(FIELD, __VA_ARGS__) \
}; }                     \
static const internal:: C##_query C;  \
}

#endif //QUERY_QUERY_HELPER_HPP
