#include <utility>

#include "matador/sql/column.hpp"
#include "matador/sql/token_visitor.hpp"

namespace matador {

column::column(std::string name, column::t_quotes_options quotes_options)
: detail::token(t_token::COLUMN)
, name_(std::move(name))
, quotes_options_(quotes_options) {}

column::column(std::string name, data_type type, size_t index,
                                         const field_attributes &attr)
: detail::token(t_token::COLUMN)
, name_(std::move(name))
, use_case_(t_use_case::CREATE_COLUMN)
, type_(type)
, attributes_(attr)
, index_(index) {}

column::column(std::string name, const any &val)
: detail::token(t_token::COLUMN)
, name_(std::move(name))
, use_case_(t_use_case::UPDATE_COLUMN)
, val_(val)
{}

void column::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

const std::string &column::name() const
{
  return name_;
}

void column::name(const std::string &n)
{
  name_ = n;
}

bool column::with_quotes() const
{
  return quotes_options_ == t_quotes_options::WITH_QUOTES;
}

bool column::without_quotes() const
{
  return quotes_options_ == t_quotes_options::WITHOUT_QUOTES;
}

bool column::is_name_column() const
{
  return use_case_ == t_use_case::COLUMN_NAME;
}

bool column::is_create_column() const
{
  return use_case_ == t_use_case::CREATE_COLUMN;
}

bool column::is_update_column() const
{
  return use_case_ == t_use_case::UPDATE_COLUMN;
}

data_type column::type() const
{
  return type_;
}

const field_attributes &column::attributes() const
{
  return attributes_;
}

size_t column::index() const
{
  return index_;
}

const struct value& column::value() const
{
  return val_;
}

struct value &column::value()
{
  return val_;
}

void column::value(const struct value &val) {
  val_.value_ = val.value_;
}

column operator "" _col(const char *name, size_t len)
{
    return column(std::string(name, len));
}

column make_pk_column(const std::string &name, data_type type, size_t index, size_t max_size)
{
  return {name, type, index, field_attributes{ max_size, constraints::PRIMARY_KEY | constraints::NOT_NULL}};
}

column make_column(const std::string &name, data_type type, size_t index, const field_attributes &attr)
{
  return {name, type, index, attr};
}

column make_column(const std::string &name, const any &val)
{
  return {name, val};
}

template<>
column make_pk_column<std::string>(const std::string &name, size_t max_size)
{
  return {name, data_type_traits<std::string>::builtin_type(max_size), max_size, field_attributes{max_size, constraints::PRIMARY_KEY | constraints::NOT_NULL}};
}

}
