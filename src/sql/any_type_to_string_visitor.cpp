#include "matador/sql/any_type_to_string_visitor.hpp"

#include "matador/sql/dialect.hpp"
#include "matador/sql/query_context.hpp"

#include "matador/utils/convert.hpp"
#include "matador/utils/string.hpp"

namespace matador::sql {

any_type_to_string_visitor::any_type_to_string_visitor(const dialect &d, query_context &query)
: d(d), query(query)
{}

void any_type_to_string_visitor::to_string(const float &val)
{
  utils::convert(result, val);
}

void any_type_to_string_visitor::to_string(const double &val)
{
  utils::convert(result, val);
}

void any_type_to_string_visitor::to_string(const char *val)
{
  result = "'" + d.prepare_literal(val) + "'";
}

void any_type_to_string_visitor::to_string(const std::string &val)
{
  result = "'" + d.prepare_literal(val) + "'";
}

void any_type_to_string_visitor::to_string(const time &val)
{
  result = "'" + d.prepare_literal(utils::to_string(val, "%FT%T.%f")) + "'";
}

void any_type_to_string_visitor::to_string(const date &val)
{
  result = "'" + d.prepare_literal(utils::to_string(val)) + "'";
}

void any_type_to_string_visitor::to_string(const utils::blob &val)
{
  // "This is a binary Data string" as binary data:
  // MySQL:    X'5468697320697320612062616E617279204461746120737472696E67'
  // Postgres: E'\\x5468697320697320612062616E617279204461746120737472696E67'
  // MSSQL:    0x5468697320697320612062616E617279204461746120737472696E67
  // Sqlite:   X'5468697320697320612062616E617279204461746120737472696E67'
  result = d.token_at(dialect_token::BEGIN_BINARY_DATA) + utils::to_string(val) + d.token_at(dialect_token::END_BINARY_DATA);
}

void any_type_to_string_visitor::to_string(const utils::placeholder &/*val*/)
{
  query.bind_vars.emplace_back("unknown");
  result = d.next_placeholder(query.bind_vars);
}

}