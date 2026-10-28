#ifndef MATADOR_SCHEMA_UTILS_HPP
#define MATADOR_SCHEMA_UTILS_HPP

#include "matador/query/schema/basic_schema.hpp"
#include "matador/query/internal/query_contexts.hpp"

namespace matador::query {
class dialect;
query_contexts to_query_contexts(const schema_node &node, const dialect &d);
}
#endif //MATADOR_SCHEMA_UTILS_HPP
