#include "matador/sql/any_type_to_string_visitor.hpp"

#include "matador/sql/dialect.hpp"
#include "matador/sql/query_context.hpp"

namespace matador::sql {

any_type_to_string_visitor::any_type_to_string_visitor(attribute_string_writer &writer, query_context &query)
: writer(&writer), query(query)
{}

void any_type_to_string_visitor::operator()(const utils::placeholder& /*x*/) {
    query.bind_vars.emplace_back("unknown");
    result = writer->dialect().next_placeholder(query.bind_vars);
}

}