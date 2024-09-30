#include "matador/sql/dialect.hpp"

#include "matador/utils/string.hpp"

namespace matador::sql {

const std::string& dialect::token_at(dialect_token token) const
{
  return tokens_.at(token);
}

const std::string &dialect::data_type_at(data_type type) const
{
  return data_types_.at(type);
}

std::string dialect::prepare_identifier(const column &col) const
{
  std::string result;
  if (!col.is_function()) {
    if (!col.table_->name.empty()) {
      result = prepare_identifier_string(col.table_->has_alias() ? col.table_->alias : col.table_->name) + ".";
    }
    result += prepare_identifier_string(col.name);
  } else {
    result = sql_func_map_.at(col.function_) + "(" + col.name + ")";
  }
  if (!col.alias.empty()) {
    result += " AS " + col.alias;
  }
  return result;
}

std::string dialect::prepare_condition(const column& col) const
{
  std::string result;
  if (!col.is_function()) {
    if (!col.alias.empty()) {
        result = col.alias;
    } else {
        if (!col.table_->name.empty()) {
            result = prepare_identifier_string(col.table_->has_alias() ? col.table_->alias : col.table_->name) + ".";
        }
        result += prepare_identifier_string(col.name);
    }
  } else {
    result = sql_func_map_.at(col.function_) + "(" + col.name + ")";
  }

  return result;
}

std::string dialect::prepare_identifier_string(const std::string &col) const
{
  auto parts = utils::split(col, '.');

  for (auto &part : parts) {
    escape_quotes_in_identifier(part);
    quote_identifier(part);
  }
  return utils::join(parts, ".");
}

std::string dialect::prepare_literal(const std::string &str) const
{
  std::string result(str);
  escape_quotes_in_literals(result);
  return result;
}

void dialect::quote_identifier(std::string &str) const
{
  str.insert(0, token_at(dialect_token::START_QUOTE));
  str += token_at(dialect_token::END_QUOTE);
}

void dialect::escape_quotes_in_identifier(std::string &str) const
{
  const std::string open_char(token_at(dialect_token::START_QUOTE));
  const std::string close_char(token_at(dialect_token::END_QUOTE));
  if (identifier_escape_type() == escape_identifier_t::ESCAPE_CLOSING_BRACKET) {
    utils::replace_all(str, close_char, close_char + close_char);
  } else {
    utils::replace_all(str, open_char, open_char + open_char);
  }
}

void dialect::escape_quotes_in_literals(std::string &str) const
{
  const std::string single_quote(token_at(dialect_token::STRING_QUOTE));
  const std::string double_quote(token_at(dialect_token::STRING_QUOTE) + token_at(dialect_token::STRING_QUOTE));
  utils::replace_all(str, single_quote, double_quote);
}

dialect::escape_identifier_t dialect::identifier_escape_type() const
{
  return escape_identifier_t::ESCAPE_BOTH_SAME;
}

std::string dialect::next_placeholder(const std::vector<std::string> &bind_vars) const
{
  return placeholder_func_(bind_vars.size());
}

std::string dialect::default_schema_name() const
{
  return default_schema_name_;
}

query_context dialect::compile(const query_compile_context& data, const connection_impl &conn) const
{
    return compiler_->compile(&data, conn);
}

void dialect::compiler(std::unique_ptr<query_compiler> &&compiler)
{
  compiler_ = std::move(compiler);
}

dialect::dialect()
: compiler_(std::make_unique<query_compiler>())
{}

}
