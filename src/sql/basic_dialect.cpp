//
// Created by sascha on 3/4/16.
//
#include "matador/sql/basic_dialect.hpp"
#include "matador/sql/basic_dialect_compiler.hpp"
#include "matador/sql/basic_dialect_linker.hpp"
#include "matador/sql/sql.hpp"

#include "matador/utils/string.hpp"

namespace matador {

namespace detail {

build_info::build_info(const sql &s, basic_dialect *d)
  : dialect(d)
{
  tokens_.assign(s.token_list_.begin(),s.token_list_.end());
  current = tokens_.begin();
}

}

basic_dialect::basic_dialect(detail::basic_dialect_compiler *compiler, detail::basic_dialect_linker *linker)
  : compiler_(compiler)
  , linker_(linker)
{
  compiler_->dialect(this);
  linker_->dialect(this);
}

basic_dialect::~basic_dialect()
{
  delete compiler_;
  delete linker_;
}

std::string basic_dialect::direct(const sql &s)
{
  return build(s, DIRECT);
}

detail::statement_context basic_dialect::prepare(const sql &s)
{
  return {build(s, PREPARED), host_vars_, columns_};
}

std::string basic_dialect::build(const sql &s, t_compile_type compile_type)
{
  compile_type_ = compile_type;
  host_vars_.clear();
  columns_.clear();

  push(s);
  compile();
  link();
  std::string result(top().result);
  pop();

  return result;
}

std::string basic_dialect::continue_build(const sql &s, t_compile_type compile_type) {
  compile_type_ = compile_type;

  push(s);
  compile();
  link();
  std::string result(top().result);
  pop();

  return result;
}

void basic_dialect::compile()
{
  compiler_->compile(*this);
}

void basic_dialect::link()
{
  linker_->link();
}

bool basic_dialect::is_preparing() const
{
  return compile_type_ == PREPARED;
}

void basic_dialect::replace_token(detail::token::t_token tkn, const std::string &value)
{
  tokens[tkn] = value;
}

void basic_dialect::append_to_result(const std::string &part)
{
  top().result += part;
}

void basic_dialect::push(const sql &s)
{
  build_info_stack_.push(detail::build_info(s, this));
}

void basic_dialect::pop()
{
  build_info_stack_.pop();
}

detail::build_info &basic_dialect::top()
{
  return build_info_stack_.top();
}

void basic_dialect::add_host_var(const std::string &host_var)
{
  host_vars_.push_back(host_var);
}

void basic_dialect::add_column(const std::string &column)
{
  columns_.push_back( column);
}

const std::vector<std::string>& basic_dialect::host_vars() const
{
  return host_vars_;
}

const std::vector<std::string>& basic_dialect::columns() const
{
  return columns_;
}

std::string basic_dialect::prepare_identifier(const std::string &str)
{
  std::string result(str);
  escape_quotes_in_identifier(result);
  quote_identifier(result);
  return result;
}

std::string basic_dialect::prepare_literal(const std::string &str) const
{
  std::string result(str);
  escape_quotes_in_literals(result);
  return result;
}

void basic_dialect::quote_identifier(std::string &str)
{
  str.insert(0, token_at(detail::token::START_QUOTE));
  str += token_at(detail::token::END_QUOTE);
}

void basic_dialect::escape_quotes_in_identifier(std::string &str)
{
  const std::string open_char(token_at(detail::token::START_QUOTE));
  std::string close_char(token_at(detail::token::END_QUOTE));
  if (identifier_escape_type() == dialect_traits::ESCAPE_CLOSING_BRACKET) {
    replace_all(str, close_char, close_char + close_char);
  } else {
    replace_all(str, open_char, open_char + open_char);
  }
}

void basic_dialect::escape_quotes_in_literals(std::string &str) const
{
  const std::string single_quote(token_at(detail::token::STRING_QUOTE));
  const std::string double_quote(token_at(detail::token::STRING_QUOTE) + token_at(detail::token::STRING_QUOTE));
  replace_all(str, single_quote, double_quote);
}

std::string basic_dialect::next_placeholder() const
{
  return "?";
}

char basic_dialect::identifier_opening_quote() const
{
  return token_at(detail::token::START_QUOTE)[0];
}

char basic_dialect::identifier_closing_quote() const
{
  return token_at(detail::token::END_QUOTE)[0];
}

std::string basic_dialect::token_at(detail::token::t_token tok) const
{
  return tokens.at(tok);
}

basic_dialect::t_compile_type basic_dialect::compile_type() const
{
  return compile_type_;
}

}