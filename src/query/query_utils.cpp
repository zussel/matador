#include "matador/query/query_utils.hpp"

#include "matador/query/internal/attribute_string_writer.hpp"
#include "matador/query/column.hpp"
#include "matador/query/query_context.hpp"
#include "matador/query/internal/string_builder_utils.hpp"
#include "matador/query/expression_evaluator.hpp"
#include "matador/query/dialect.hpp"

#include "matador/utils/string.hpp"
#include "matador/query/column_value.hpp"

namespace matador::query {
void prepare_column(query_context& ctx, const dialect& d, const column& col) {
    // Expression-backed select item: (<expr>) [AS alias]
    if (col.is_expression()) {
        attribute_string_writer writer(d, {});
        expression_evaluator v(d, ctx);
        col.expression()->accept(v);
      ctx.sql += v.result();

        if (col.has_alias()) {
            ctx.sql.append(" ").append(d.as()).append(" ").append(col.alias());
        }
    } else {
        prepare_column(ctx.sql, d, col);
    }
}

void prepare_column(std::string &out, const dialect &d, const column &col) {
  if (!col.is_function()) {
    prepare_identifier_string_append(out, col.name(), d);
  } else {
    if (col.column_name() == d.asterisk()) {
      out += d.sql_function_at(col.function()) + "(" + col.column_name() + ")";
    } else {
      out += d.sql_function_at(col.function()) + "(" + col.column_name() + ") " + d.as() + " " + col.alias();
    }
  }
}

std::string prepare_identifier(const dialect& d, const column& col) {
  std::string result;
  if (!col.is_function()) {
    prepare_identifier_string_append(result, col.name(), d);
  } else {
    if (col.column_name() == d.asterisk()) {
      result = d.sql_function_at(col.function()) + "(" + col.column_name() + ")";
    } else {
      result = d.sql_function_at(col.function()) + "(" + col.column_name() + ") " + d.as() + " " + col.alias();
    }
  }
  return result;
}

std::string prepare_criteria(const dialect& d, const column& col) {
  std::string result;
  if (!col.is_function()) {
    prepare_identifier_string_append(result, col.name(), d);
  } else {
    if (col.column_name() == d.asterisk()) {
      result = d.sql_function_at(col.function()) + "(" + col.column_name() + ")";
    } else {
      result = d.sql_function_at(col.function()) + "(" + col.column_name() + ") " + d.as() + " " + col.alias();
    }
  }

  return result;
}

std::string to_query_string(const column_value &val, const dialect& d) {
  if (val.is_null()) {
    return "NULL";
  }
  if (val.is_string()) {
    return d.token_at(dialect_token::BeginStringData) + val.str() + d.token_at(dialect_token::BeginStringData);
  }

  return val.str();
}

std::string prepare_literal(const std::string &str, const dialect& d) {
  std::string result(str);
  escape_quotes_in_literals(result, d);
  return result;
}

std::string prepare_identifier_string(const std::string &col, const dialect& d) {
  auto parts = utils::split(col, '.');

  for (auto &part: parts) {
    escape_quotes_in_identifier(part, d);
    part.insert(0, d.token_at(dialect_token::StartQuote));
    part += d.token_at(dialect_token::EndQuote);
  }

  return utils::join(parts, ".");
}

void escape_quotes_in_identifier(std::string &str, const dialect& d) {
  const std::string& open_char(d.token_at(dialect_token::StartQuote));
  const std::string& close_char(d.token_at(dialect_token::EndQuote));
  if (d.identifier_escape_type() == dialect::escape_identifier_t::EscapeClosingBracket) {
    utils::replace_all(str, close_char, close_char + close_char);
  } else {
    utils::replace_all(str, open_char, open_char + open_char);
  }
}

void escape_quotes_in_literals(std::string &str, const dialect& d) {
  const std::string& single_quote_char(d.token_at(dialect_token::StringQuote));
  const std::string double_quote(single_quote_char + single_quote_char);
  utils::replace_all(str, single_quote_char, double_quote);
}

}