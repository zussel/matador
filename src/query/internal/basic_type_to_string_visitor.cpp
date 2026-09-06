#include "matador/query/internal/basic_type_to_string_visitor.hpp"

namespace matador::query::internal {

basic_type_to_string_visitor::basic_type_to_string_visitor(attribute_string_writer &writer, query_context &query)
: writer(&writer), query(query)
{}

}